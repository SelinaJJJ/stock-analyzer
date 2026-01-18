-- Schema for prices, signals, and run metadata
-- Ensure uuid extension exists for gen_random_uuid()
CREATE EXTENSION IF NOT EXISTS pgcrypto;
CREATE TABLE IF NOT EXISTS prices (
  symbol TEXT NOT NULL,
  date DATE NOT NULL,
  open DOUBLE PRECISION,
  high DOUBLE PRECISION,
  low DOUBLE PRECISION,
  close DOUBLE PRECISION,
  adj_close DOUBLE PRECISION,
  volume BIGINT,
  PRIMARY KEY (symbol, date)
);

CREATE TABLE IF NOT EXISTS runs (
  run_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  started_at TIMESTAMP WITH TIME ZONE DEFAULT now(),
  finished_at TIMESTAMP WITH TIME ZONE,
  universe TEXT,
  params_json JSONB,
  metrics_json JSONB
);

CREATE TABLE IF NOT EXISTS signals (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  symbol TEXT NOT NULL,
  as_of_date DATE NOT NULL,
  horizon TEXT NOT NULL,
  trend_label TEXT,
  score DOUBLE PRECISION,
  prob_up DOUBLE PRECISION,
  expected_return DOUBLE PRECISION,
  volatility DOUBLE PRECISION,
  drawdown DOUBLE PRECISION,
  model_version TEXT,
  run_id UUID REFERENCES runs(run_id)
);

-- Indexes for fast queries
CREATE INDEX IF NOT EXISTS idx_prices_symbol_date ON prices(symbol, date DESC);
CREATE INDEX IF NOT EXISTS idx_signals_symbol_date ON signals(symbol, as_of_date DESC);
