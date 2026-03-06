import { useState } from 'react';
import './index.css';

function App() {
  const [specification, setSpecification] = useState('');
  const [generatedCode, setGeneratedCode] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleGenerate = async () => {
    if (!specification.trim()) return;

    setIsLoading(true);
    setError(null);

    try {
      const response = await fetch('http://localhost:8080/api/generate-rtl', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ specification }),
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }

      const data = await response.json();
      setGeneratedCode(data.generatedCode || 'No code generated');
      
      // We could also show validation status here
      // console.log("Validation Status:", data.validationStatus);
      
    } catch (err) {
      setError(err.message);
      console.error("Error generating RTL:", err);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="app-container dark-theme">
      {/* HEADER */}
      <header className="header">
        <h1>NeuralNest</h1>
        <span className="subtitle">AI-Driven RTL Autopilot</span>
      </header>
      
      {/* COCKPIT SPLIT SCREEN */}
      <main className="cockpit">
        
        {/* LEFT PANE: INPUT */}
        <section className="pane input-pane">
          <h2>Hardware Specification</h2>
          <textarea
            className="spec-textarea"
            placeholder="Describe your RTL design (e.g., 'A synchronous D flip-flop with active-low reset...')"
            value={specification}
            onChange={(e) => setSpecification(e.target.value)}
          />
          <button 
            className="generate-btn" 
            onClick={handleGenerate}
            disabled={isLoading || !specification.trim()}
          >
            {isLoading ? 'Generating Engine...' : 'Generate RTL'}
          </button>
          
          {error && <div className="error-message">Error: {error}</div>}
        </section>

        {/* RIGHT PANE: CODE DISPLAY */}
        <section className="pane code-pane">
          <h2>Generated Verilog</h2>
          <div className="editor-container">
            <pre className="code-display">
              <code>{generatedCode || '// Code will appear here...'}</code>
            </pre>
          </div>
        </section>
        
      </main>
    </div>
  );
}

export default App;
