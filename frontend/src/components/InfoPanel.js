import './InfoPanel.css';

function InfoPanel({ onDismiss }) {
  return (
    <div className="info-panel">
      <button className="info-dismiss" onClick={onDismiss} title="Dismiss">
        &times;
      </button>

      <div className="info-header">
        <h3>Welcome to StockPulse</h3>
        <p>Your quantitative stock analysis dashboard</p>
      </div>

      <div className="info-grid">
        <div className="info-card">
          <div className="info-icon bullish">
            <svg viewBox="0 0 24 24" fill="none">
              <path d="M12 19V5M12 5L5 12M12 5L19 12" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"/>
            </svg>
          </div>
          <div className="info-content">
            <h4>Bullish Signal</h4>
            <p>Stock price is trending upward. The price is above key moving averages with positive momentum.</p>
          </div>
        </div>

        <div className="info-card">
          <div className="info-icon bearish">
            <svg viewBox="0 0 24 24" fill="none">
              <path d="M12 5V19M12 19L5 12M12 19L19 12" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"/>
            </svg>
          </div>
          <div className="info-content">
            <h4>Bearish Signal</h4>
            <p>Stock price is trending downward. The price is below key moving averages with negative momentum.</p>
          </div>
        </div>

        <div className="info-card">
          <div className="info-icon neutral">
            <svg viewBox="0 0 24 24" fill="none">
              <path d="M5 12H19" stroke="currentColor" strokeWidth="2" strokeLinecap="round"/>
            </svg>
          </div>
          <div className="info-content">
            <h4>Neutral / Hold</h4>
            <p>No clear trend direction. The stock is consolidating or moving sideways.</p>
          </div>
        </div>

        <div className="info-card">
          <div className="info-icon probability">
            <svg viewBox="0 0 24 24" fill="none">
              <circle cx="12" cy="12" r="9" stroke="currentColor" strokeWidth="2"/>
              <path d="M12 8V12L15 14" stroke="currentColor" strokeWidth="2" strokeLinecap="round"/>
            </svg>
          </div>
          <div className="info-content">
            <h4>Bullish Probability</h4>
            <p>Likelihood the stock will go up in the next 30 days based on historical patterns and current trends.</p>
          </div>
        </div>
      </div>

      <div className="info-terms">
        <h4>Key Terms Explained</h4>
        <div className="terms-grid">
          <div className="term">
            <span className="term-name">MA5</span>
            <span className="term-desc">5-day Moving Average - average price over the last 5 trading days</span>
          </div>
          <div className="term">
            <span className="term-name">MA20</span>
            <span className="term-desc">20-day Moving Average - average price over the last 20 trading days</span>
          </div>
          <div className="term">
            <span className="term-name">Volatility</span>
            <span className="term-desc">How much the stock price fluctuates - higher means more risk but also more opportunity</span>
          </div>
          <div className="term">
            <span className="term-name">Monthly Return</span>
            <span className="term-desc">Percentage change in price over the last ~30 trading days</span>
          </div>
        </div>
      </div>

      <div className="info-footer">
        <p>Click the <strong>?</strong> button anytime to show/hide this guide</p>
      </div>
    </div>
  );
}

export default InfoPanel;
