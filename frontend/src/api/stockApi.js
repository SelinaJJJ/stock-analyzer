const API_BASE = '/api';

export async function fetchSignals(date) {
  const url = date ? `${API_BASE}/signals?date=${date}` : `${API_BASE}/signals`;
  const response = await fetch(url);

  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }

  return response.json();
}

export async function fetchUniverses() {
  const response = await fetch(`${API_BASE}/universe`);

  if (!response.ok) {
    throw new Error(`HTTP error! status: ${response.status}`);
  }

  return response.json();
}
