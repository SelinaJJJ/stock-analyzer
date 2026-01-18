import './Header.css';

function Header() {
  return (
    <header className="header">
      <div className="header-content">
        <div className="logo">
          <div className="logo-icon">
            <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
              <path d="M3 17L9 11L13 15L21 7" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round"/>
              <path d="M17 7H21V11" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round"/>
            </svg>
          </div>
          <div className="logo-text">
            <h1>StockPulse</h1>
            <span className="tagline">Quantitative Analysis</span>
          </div>
        </div>
        <nav className="nav">
          <span className="nav-item active">Signals</span>
          <span className="nav-item">Portfolio</span>
          <span className="nav-item">Analytics</span>
        </nav>
        <div className="header-status">
          <span className="status-dot"></span>
          <span className="status-text">Live</span>
        </div>
      </div>
    </header>
  );
}

export default Header;
