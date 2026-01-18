import './SignalCard.css';

// Stock company names for better display
const stockNames = {
  AAPL: 'Apple Inc.',
  MSFT: 'Microsoft Corp.',
  GOOG: 'Alphabet Inc.',
  GOOGL: 'Alphabet Inc.',
  AMZN: 'Amazon.com Inc.',
  NVDA: 'NVIDIA Corp.',
  META: 'Meta Platforms',
  TSLA: 'Tesla Inc.',
  BRK: 'Berkshire Hathaway',
  JPM: 'JPMorgan Chase',
  V: 'Visa Inc.',
  JNJ: 'Johnson & Johnson',
  WMT: 'Walmart Inc.',
  PG: 'Procter & Gamble',
  MA: 'Mastercard Inc.',
  UNH: 'UnitedHealth Group',
  HD: 'Home Depot',
  DIS: 'Walt Disney Co.',
  BAC: 'Bank of America',
  XOM: 'Exxon Mobil'
};

function SignalCard({ signal }) {
  const trendColors = {
    strong_up: 'bullish',
    up: 'bullish',
    neutral: 'neutral',
    down: 'bearish',
    strong_down: 'bearish',
    unknown: 'neutral',
    insufficient_data: 'neutral'
  };

  const trendLabels = {
    strong_up: 'Strong Buy',
    up: 'Buy',
    neutral: 'Hold',
    down: 'Sell',
    strong_down: 'Strong Sell',
    unknown: 'Unknown',
    insufficient_data: 'No Data'
  };

  const trendDescriptions = {
    strong_up: 'Strong upward momentum. Price is above both MA5 and MA20 with solid gains.',
    up: 'Positive trend. Price is trading above the 20-day moving average.',
    neutral: 'No clear direction. Consider waiting for a clearer signal.',
    down: 'Negative trend. Price is trading below the 20-day moving average.',
    strong_down: 'Strong downward momentum. Price is below both MA5 and MA20 with losses.',
    unknown: 'Unable to determine trend due to data issues.',
    insufficient_data: 'Not enough historical data to calculate trend.'
  };

  const colorClass = trendColors[signal.trend] || 'neutral';
  const trendLabel = trendLabels[signal.trend] || signal.trend;
  const trendDescription = trendDescriptions[signal.trend] || '';
  const probability = ((signal.prob_up || 0.5) * 100).toFixed(0);
  const monthlyReturn = signal.monthly_return || 0;
  const currentPrice = signal.current_price || 0;
  const volatility = signal.volatility || 0;
  const companyName = stockNames[signal.symbol] || 'Stock';

  return (
    <div className={`signal-card ${colorClass}`}>
      <div className="card-header">
        <div className="symbol-section">
          <span className="symbol">{signal.symbol}</span>
          <span className="company-name">{companyName}</span>
          <span className={`trend-badge ${colorClass}`} title={trendDescription}>
            {trendLabel}
          </span>
        </div>
        {currentPrice > 0 && (
          <div className="price-section">
            <span className="current-price">${currentPrice.toFixed(2)}</span>
            <span
              className={`price-change ${monthlyReturn >= 0 ? 'positive' : 'negative'}`}
              title="30-day price change"
            >
              {monthlyReturn >= 0 ? '+' : ''}{monthlyReturn.toFixed(2)}%
            </span>
          </div>
        )}
      </div>

      {trendDescription && (
        <p className="trend-description">{trendDescription}</p>
      )}

      <div className="card-body">
        <div className="probability-section">
          <div className="prob-header">
            <span className="prob-label" title="Estimated likelihood the stock will increase in the next 30 days">
              Bullish Probability
              <span className="info-hint">?</span>
            </span>
            <span className="prob-value">{probability}%</span>
          </div>
          <div className="prob-bar" title="Red = bearish, Yellow = neutral, Green = bullish">
            <div className="prob-fill" style={{ width: `${probability}%` }} />
            <div className="prob-marker" style={{ left: `${probability}%` }} />
          </div>
          <div className="prob-labels">
            <span>Bearish</span>
            <span>Bullish</span>
          </div>
        </div>

        {(signal.ma5 || signal.ma20 || volatility > 0) && (
          <div className="metrics-grid">
            {signal.ma5 > 0 && (
              <div className="metric" title="5-day Moving Average: Average closing price over the last 5 trading days">
                <span className="metric-label">MA5</span>
                <span className="metric-value">${signal.ma5.toFixed(2)}</span>
              </div>
            )}
            {signal.ma20 > 0 && (
              <div className="metric" title="20-day Moving Average: Average closing price over the last 20 trading days">
                <span className="metric-label">MA20</span>
                <span className="metric-value">${signal.ma20.toFixed(2)}</span>
              </div>
            )}
            {volatility > 0 && (
              <div className="metric" title="Annualized volatility: Higher values indicate more price fluctuation and risk">
                <span className="metric-label">Volatility</span>
                <span className={`metric-value ${volatility > 40 ? 'high-vol' : volatility > 25 ? 'med-vol' : 'low-vol'}`}>
                  {volatility.toFixed(1)}%
                </span>
              </div>
            )}
            {signal.data_points > 0 && (
              <div className="metric" title="Number of historical data points used for analysis">
                <span className="metric-label">Data Points</span>
                <span className="metric-value">{signal.data_points}</span>
              </div>
            )}
          </div>
        )}
      </div>

      {signal.error && (
        <div className="card-error">
          <span className="error-icon">!</span>
          <span className="error-text">{signal.error}</span>
        </div>
      )}
    </div>
  );
}

export default SignalCard;
