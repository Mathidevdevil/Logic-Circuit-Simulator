package com.neuralnest.controller;

import com.neuralnest.model.RtlDesign;
import com.neuralnest.service.AiOrchestratorService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*") // For development purposes
public class RtlController {

    private final AiOrchestratorService orchestratorService;

    public RtlController(AiOrchestratorService orchestratorService) {
        this.orchestratorService = orchestratorService;
    }

    @PostMapping("/generate-rtl")
    public ResponseEntity<RtlDesign> generateRtl(@RequestBody RtlRequest request) {
        RtlDesign generatedDesign = orchestratorService.generateRtl(request.getSpecification());
        return ResponseEntity.ok(generatedDesign);
    }

    static class RtlRequest {
        private String specification;

        public String getSpecification() {
            return specification;
        }

        public void setSpecification(String specification) {
            this.specification = specification;
        }
    }
}
