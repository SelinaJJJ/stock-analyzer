import React, { useState, useEffect } from 'react';
import './App.css';
import Header from './components/Header';
import SignalCard from './components/SignalCard';
import UniverseSelector from './components/UniverseSelector';
import StatsPanel from './components/StatsPanel';
import InfoPanel from './components/InfoPanel';
import { fetchSignals, fetchUniverses } from './api/stockApi';

function App() {
  const [signals, setSignals] = useState(null);
  const [universes, setUniverses] = useState([]);
  const [selectedUniverse, setSelectedUniverse] = useState('SP500');
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [selectedDate, setSelectedDate] = useState(new Date().toISOString().split('T')[0]);
  const [showHelp, setShowHelp] = useState(true);

  useEffect(() => {
    loadUniverses();
  }, []);

  useEffect(() => {
    loadSignals();
  }, [selectedDate]);

  const loadUniverses = async () => {
    try {
      const data = await fetchUniverses();
      setUniverses(data);
    } catch (err) {
      console.error('Failed to load universes:', err);
    }
  };

  const loadSignals = async () => {
    setLoading(true);
    setError(null);
    try {
      const data = await fetchSignals(selectedDate);
      setSignals(data);
    } catch (err) {
      setError('Failed to load signals. Is the backend running?');
      console.error('Failed to load signals:', err);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="app">
      <Header />

      <main className="main-content">
        {showHelp && <InfoPanel onDismiss={() => setShowHelp(false)} />}

        <div className="controls-bar">
          <UniverseSelector
            universes={universes}
            selected={selectedUniverse}
            onSelect={setSelectedUniverse}
          />

          <div className="date-picker">
            <label htmlFor="signal-date">Signal Date:</label>
            <input
              type="date"
              id="signal-date"
              value={selectedDate}
              onChange={(e) => setSelectedDate(e.target.value)}
              className="date-input"
            />
          </div>

          <button
            className="help-btn"
            onClick={() => setShowHelp(!showHelp)}
            title="Toggle help panel"
          >
            ?
          </button>

          <button className="refresh-btn" onClick={loadSignals} disabled={loading}>
            {loading ? 'Loading...' : 'Refresh'}
          </button>
        </div>

        {error && (
          <div className="error-banner">
            <span className="error-icon">!</span>
            {error}
          </div>
        )}

        <StatsPanel signals={signals} loading={loading} />

        <section className="signals-section">
          <div className="section-header-wrap">
            <h2 className="section-title">Trading Signals</h2>
            <p className="section-subtitle">
              Analysis based on price trends, moving averages, and momentum indicators
            </p>
          </div>

          {loading ? (
            <div className="loading-grid">
              {[1, 2, 3].map((i) => (
                <div key={i} className="signal-card skeleton" />
              ))}
            </div>
          ) : signals?.signals?.length > 0 ? (
            <div className="signals-grid">
              {signals.signals.map((signal, index) => (
                <SignalCard key={index} signal={signal} />
              ))}
            </div>
          ) : (
            <div className="empty-state">
              <p>No signals available for this date.</p>
              <p className="hint">Try selecting a different date or check if the backend is running.</p>
            </div>
          )}
        </section>

        {signals && (
          <footer className="data-footer">
            <span>As of: {signals.as_of}</span>
            <span>Horizon: {signals.horizon}</span>
            <span className="disclaimer">
              This is not financial advice. Always do your own research.
            </span>
          </footer>
        )}
      </main>
    </div>
  );
}

export default App;
