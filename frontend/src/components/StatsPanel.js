import './StatsPanel.css';

function StatsPanel({ signals, loading }) {
  if (loading) {
    return (
      <div className="stats-panel">
        <div className="stat-card skeleton-stat" />
        <div className="stat-card skeleton-stat" />
        <div className="stat-card skeleton-stat" />
        <div className="stat-card skeleton-stat" />
      </div>
    );
  }

  const signalList = signals?.signals || [];
  const bullishCount = signalList.filter(s =>
    s.trend === 'up' || s.trend === 'strong_up'
  ).length;
  const bearishCount = signalList.filter(s =>
    s.trend === 'down' || s.trend === 'strong_down'
  ).length;
  const avgProbability = signalList.length > 0
    ? (signalList.reduce((sum, s) => sum + (s.prob_up || 0.5), 0) / signalList.length * 100).toFixed(0)
    : 0;

  return (
    <div className="stats-panel">
      <div className="stat-card">
        <div className="stat-icon signals-icon">
          <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M3 3V21H21" stroke="currentColor" strokeWidth="2" strokeLinecap="round"/>
            <path d="M7 14L12 9L15 12L21 6" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"/>
          </svg>
        </div>
        <div className="stat-content">
          <span className="stat-value">{signalList.length}</span>
          <span className="stat-label">Total Signals</span>
        </div>
      </div>

      <div className="stat-card">
        <div className="stat-icon bullish-icon">
          <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M12 19V5M12 5L5 12M12 5L19 12" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"/>
          </svg>
        </div>
        <div className="stat-content">
          <span className="stat-value bullish">{bullishCount}</span>
          <span className="stat-label">Bullish</span>
        </div>
      </div>

      <div className="stat-card">
        <div className="stat-icon bearish-icon">
          <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M12 5V19M12 19L5 12M12 19L19 12" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"/>
          </svg>
        </div>
        <div className="stat-content">
          <span className="stat-value bearish">{bearishCount}</span>
          <span className="stat-label">Bearish</span>
        </div>
      </div>

      <div className="stat-card">
        <div className="stat-icon prob-icon">
          <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <circle cx="12" cy="12" r="9" stroke="currentColor" strokeWidth="2"/>
            <path d="M12 7V12L15 15" stroke="currentColor" strokeWidth="2" strokeLinecap="round"/>
          </svg>
        </div>
        <div className="stat-content">
          <span className="stat-value">{avgProbability}%</span>
          <span className="stat-label">Avg Probability</span>
        </div>
      </div>
    </div>
  );
}

export default StatsPanel;
