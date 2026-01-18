# Quant Market Trends & Stock Signal Analyzer

Opinionated scaffold for a Quant trading signals project using a C++ quant core and Java Spring Boot platform.

Quick contents

- `docker-compose.yml` - Postgres + Redis for local development
- `sql/schema.sql` - DB schema for `prices`, `signals`, `runs`
- `backend/` - Java (Spring Boot) service skeleton (Gradle)
- `cpp/` - C++ Quant Core skeleton (CMake)
- `docs/architecture.md` - Project plan and phases

Getting started (recommended minimal steps)

1. Start local infra (Postgres + Redis):

   docker compose up -d

2. Initialize DB:

   # The Postgres container will run init scripts on first startup. If you need to
   # initialize manually, run:
   psql postgresql://postgres:postgres@localhost:5432/stockdb -f sql/schema.sql

3. Java backend

   - From `backend/` run the helper script (it prefers the Gradle wrapper if present):

      ```bash
      cd backend
      ./run-backend.sh
      ```

   - Alternatively run `gradle bootRun` if you have Gradle installed system-wide.

4. C++ Quant Core

   - Build and run unit tests:

     ```bash
     mkdir -p cpp/build && cd cpp/build
     cmake ..
     make
     ctest --output-on-failure
     ```

   - Run sample: `./quant_core ../data/sample_prices.csv` (you can export from Postgres as CSV)
    - Run sample: `./quant_core ../data/sample_prices.csv` (you can export from Postgres as CSV)

   CSV upload endpoint

   You can bulk-load price data via the backend CSV upload endpoint. The CSV must have a header like:

   symbol,date,open,high,low,close,adj_close,volume

   Example curl to upload:

   ```bash
   curl -X POST -F "file=@prices_combined.csv" http://localhost:8080/api/upload/prices
   ```

   Response will be a JSON summary with `processed` and `errors`.

Next steps

- Wire Java scheduled job to pull data and upsert into `prices`.
- Implement more indicators in `cpp/` and add CSV / Postgres connectors.
- Add unit tests and CI.

See `docs/architecture.md` for the full phased plan and recommended APIs.
