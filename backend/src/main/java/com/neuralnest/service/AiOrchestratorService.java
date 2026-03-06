package com.neuralnest.service;

import com.neuralnest.model.RtlDesign;
import com.neuralnest.repository.RtlDesignRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.UUID;

@Service
public class AiOrchestratorService {

    private static final Logger log = LoggerFactory.getLogger(AiOrchestratorService.class);
    private final RtlDesignRepository repository;

    public AiOrchestratorService(RtlDesignRepository repository) {
        this.repository = repository;
    }

    private static final String SYSTEM_PROMPT = """
            You are the core intelligence engine of NeuralNest, an advanced, AI-driven RTL Autopilot. Your sole purpose is to write highly optimized, synthesizable Verilog-2001 code based on natural language specifications.

            Strict Directives:
            Output Format: Output ONLY raw Verilog code. Do not include markdown formatting, backticks, conversational text, explanations, or comments outside the code block.
            Design Rules: Ensure all code is strictly synthesizable. Use synchronous resets unless otherwise specified. Clearly separate sequential logic (always @(posedge clk)) from combinational logic (always @*).
            Naming Conventions: Use clear, industry-standard naming (e.g., clk, rst_n, *_i for inputs, *_o for outputs, *_q for registers).
            Optimization: Prioritize minimizing gate count and critical path delay.
            Self-Correction: If you receive an error traceback from a previous compilation attempt, analyze the syntax error and output a fully corrected Verilog module.
            """;

    public RtlDesign generateRtl(String specification) {
        log.info("Received specification: {}", specification);

        // 1. Construct full prompt (System Prompt + User Spec)
        String fullPrompt = SYSTEM_PROMPT + "\nUser Specification:\n" + specification;

        // 2. Mock LLM API Call
        String generatedVerilog = mockLlmApiCall(fullPrompt);

        // 3. Validate generated code
        String validationStatus = validateVerilogCode(generatedVerilog);

        // 4. Save to Database
        RtlDesign design = new RtlDesign();
        design.setSpecification(specification);
        design.setGeneratedCode(generatedVerilog);
        design.setValidationStatus(validationStatus);

        return repository.save(design);
    }

    private String mockLlmApiCall(String prompt) {
        // Simulating API delay
        try {
            Thread.sleep(1500);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        // Extremely simple mocked generation based on keywords, just for demo purposes.
        if (prompt.toLowerCase().contains("d flip flop")) {
            return """
                    module dff (
                        input wire clk,
                        input wire rst_n,
                        input wire d_i,
                        output reg q_o
                    );

                    always @(posedge clk or negedge rst_n) begin
                        if (!rst_n) begin
                            q_o <= 1'b0;
                        end else begin
                            q_o <= d_i;
                        end
                    end

                    endmodule
                    """;
        }

        return """
                module mock_module (
                    input wire clk,
                    input wire rst_n,
                    output reg test_o
                );

                always @(posedge clk) begin
                    if (!rst_n) begin
                        test_o <= 1'b0;
                    end else begin
                        test_o <= 1'b1;
                    end
                end

                endmodule
                """;
    }

    private String validateVerilogCode(String verilogCode) {
        try {
            // Write verilog to a temporary file
            String tempFileName = "temp_" + UUID.randomUUID().toString() + ".v";
            File tempDir = new File(System.getProperty("java.io.tmpdir"), "neuralnest_scripts");
            tempDir.mkdirs();
            File tempFile = new File(tempDir, tempFileName);

            try (FileWriter writer = new FileWriter(tempFile)) {
                writer.write(verilogCode);
            }

            // Build path dynamically based on environment (Docker vs Local)
            String scriptPath = new File("scripts/verify_rtl.py").getAbsolutePath();
            if (new File("/app/scripts/verify_rtl.py").exists()) {
                scriptPath = "/app/scripts/verify_rtl.py";
            }

            // Using python3 explicitly to avoid alias issues in some Linux environments
            ProcessBuilder pb = new ProcessBuilder("python3", scriptPath, tempFile.getAbsolutePath());
            Process p = pb.start();

            StringBuilder output = new StringBuilder();
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
                    BufferedReader errorReader = new BufferedReader(new InputStreamReader(p.getErrorStream()))) {

                String line;
                while ((line = reader.readLine()) != null) {
                    output.append(line).append("\\n");
                }
                while ((line = errorReader.readLine()) != null) {
                    output.append(line).append("\\n");
                }
            }

            int exitCode = p.waitFor();

            // Clean up temporary file
            tempFile.delete();

            if (exitCode == 0) {
                return "PASS";
            } else {
                return "FAIL: " + output.toString();
            }

        } catch (Exception e) {
            log.error("Error validating Verilog: ", e);
            return "VALIDATION_ERROR: " + e.getMessage();
        }
    }
}
