import React from 'react';
import './UniverseSelector.css';

function UniverseSelector({ universes, selected, onSelect }) {
  return (
    <div className="universe-selector">
      <label className="selector-label">Universe:</label>
      <div className="universe-buttons">
        {universes.map((universe) => (
          <button
            key={universe}
            className={`universe-btn ${selected === universe ? 'active' : ''}`}
            onClick={() => onSelect(universe)}
          >
            {universe}
          </button>
        ))}
      </div>
    </div>
  );
}

export default UniverseSelector;
