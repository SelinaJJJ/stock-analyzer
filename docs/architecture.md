# Architecture & Build Plan

This document summarizes the recommended architecture and phased plan for the Quant Market Trends & Stock Signal Analyzer.

High level

- C++ Quant Core: high-performance feature engineering and indicator computation.
- Java (Spring Boot): scheduling, REST APIs, persistence (Postgres), cache (Redis), WebSocket streaming.

Phases (short):

Phase 1 — Data ingestion & storage
- DB schema (see `sql/schema.sql`)
- Java scheduled job fetches daily bars and upserts to `prices`.

Phase 2 — C++ Quant Core
- Candle and Series primitives
- Indicator module (EMA, RSI, MACD, ATR, realized vol)
- Feature extractor producing CSV or direct DB writes

Phase 3 — Prediction layer
- Logistic regression (interpretable) for P(forward_20d > 0)
- Optionally XGBoost later

Phase 4 — Backtesting
- Walk-forward validation, AUC, IC, top-decile spread

Phase 5 — API & Dashboard
- Endpoints: /universe, /signals, /symbol/{ticker}/history, /symbol/{ticker}/explain

Design notes

- Keep the C++ code as a library with a small CLI. Input: CSV or Postgres export. Output: CSV signals.
- The Java platform schedules runs and stores run metadata and final signals. Keep models and model_version in `signals`.
- For reproducible runs, store `params_json` and `metrics_json` per run.
