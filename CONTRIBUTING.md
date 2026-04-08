# Contributing to BinGo

Thank you for your interest in contributing to BinGo! This document provides guidelines and instructions for contributing.

## Ways to Contribute

- **Bug Reports** — Found a bug? Open an issue with steps to reproduce it.
- **Feature Requests** — Have an idea for a new feature? Open an issue to discuss it.
- **Code Contributions** — Fix bugs, improve the UI, or add new features.
- **Documentation** — Improve docs, add examples, or fix typos.

## Getting Started

### Prerequisites

- Node.js (for the server)
- Arduino IDE or PlatformIO (for ESP32 firmware)
- A modern browser (Chrome, Edge, or Opera for Bluetooth support)

### Setup Development Environment

1. Fork the repository
2. Clone your fork:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Bingo-Bin.git
   ```
3. Install server dependencies:
   ```bash
   cd web/server
   npm install
   ```

### Running Locally

```bash
cd web/server
npm start
```

Open `http://localhost:3000` to test your changes.

## Making Changes

### 1. Create a Branch

Create a feature branch from `main`:

```bash
git checkout -b feature/your-feature-name
```

### 2. Make Your Changes

- Keep changes focused and atomic
- Follow existing code style
- Test your changes thoroughly

### 3. Commit Your Changes

Write clear, concise commit messages:

```bash
git commit -m "Add feature: checkpoint history log"
```

### 4. Push and Create PR

```bash
git push origin feature/your-feature-name
```

Then open a Pull Request on GitHub.

## Code Guidelines

### HTML/CSS

- Use semantic HTML elements
- Follow BEM naming convention for CSS classes
- Keep styles in external CSS files (`/css/`)
- Use CSS custom properties for theming

### JavaScript

- Use vanilla JS (no frameworks)
- Keep page-specific logic in `/js/` directory
- Use meaningful variable and function names
- Add comments for complex logic

### Testing

- Test WiFi mode changes against the Node.js server
- Test Bluetooth mode in Chrome/Edge (Web Bluetooth support required)
- Test on both desktop and mobile devices

## Project Structure

```
web/
├── index.html          # Home page
├── css/
│   ├── main.css        # Home page styles
│   ├── wifi.css        # WiFi page styles
│   └── bluetooth.css   # Bluetooth page styles
├── js/
│   ├── wifi.js         # WiFi page logic
│   └── bluetooth.js    # Bluetooth page logic
├── wifi/
│   └── index.html      # WiFi control panel
└── bluetooth/
    └── index.html      # Bluetooth control panel
```

## Reporting Bugs

When reporting bugs, include:

- Clear description of the issue
- Steps to reproduce
- Expected vs actual behavior
- Browser and device information
- Error messages or screenshots if applicable

## Questions?

Feel free to open an issue for any questions about contributing.
