//
// Created by sparcman on 25. 1. 25.
//

#ifndef WEBSOCKET_H
#define WEBSOCKET_H
// 웹 페이지 HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>Weather Clock Web Terminal</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        :root {
            --bg-color: #f0f0f0;
            --text-color: #333;
            --terminal-bg: #000;
            --terminal-text: #00ff00;
            --timestamp-color: #00BFFF;
        }

        [data-theme="dark"] {
            --bg-color: #1a1a1a;
            --text-color: #fff;
            --terminal-bg: #000;
            --terminal-text: #00ff00;
            --timestamp-color: #00BFFF;
        }

        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background: var(--bg-color);
            color: var(--text-color);
            transition: background-color 0.3s, color 0.3s;
        }
        .status-bar {
            background: #333;
            color: white;
            padding: 8px 15px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-radius: 4px;
        }
        .wifi-icon {
            font-family: "Segoe UI Symbol", "Arial Unicode MS", sans-serif;
            margin-right: 5px;
            display: inline-block;
        }
        .terminal {
            background-color: var(--terminal-bg);
            color: var(--terminal-text);
            font-family: monospace;
            padding: 10px;
            height: 400px;
            overflow-y: scroll;
            margin: 10px 0;
            border-radius: 4px;
            white-space: pre-wrap;
            font-size: 14px;
        }
        .timestamp {
            color: var(--timestamp-color);
        }
        .input-container {
            margin: 10px 0;
        }
        input[type="text"] {
            width: 100%;
            padding: 8px;
            margin: 5px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
            background: var(--bg-color);
            color: var(--text-color);
        }
        .button-container {
            display: flex;
            gap: 10px;
            margin-top: 10px;
            flex-wrap: wrap;
        }
        .controls {
            display: flex;
            gap: 10px;
            align-items: center;
            margin: 10px 0;
            flex-wrap: wrap;
        }
        .control-group {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        button {
            padding: 8px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            color: white;
            min-width: 100px;
        }
        #sendBtn { background-color: #4CAF50; }
        #clearBtn { background-color: #f44336; }
        #disconnectBtn { background-color: #2196F3; }
        #logViewerBtn { background-color: #FF9800; }
        button:hover { opacity: 0.8; }
        .no-results {
            color: #888;
            text-align: center;
            padding: 20px;
            font-style: italic;
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 50px;
            height: 24px;
        }
        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 24px;
        }
        .slider:before {
            position: absolute;
            content: "";
            height: 16px;
            width: 16px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        input:checked + .slider {
            background-color: #2196F3;
        }
        input:checked + .slider:before {
            transform: translateX(26px);
        }
        select {
            padding: 4px 8px;
            border-radius: 4px;
            background: var(--bg-color);
            color: var(--text-color);
        }
    </style>
</head>
<body>
    <div class="status-bar">
        <div>
            <span class="wifi-icon">&#8960;</span>
            <span id="wifi-signal">Signal: --</span>
        </div>
        <div id="websocket-status">WebSocket: --</div>
    </div>

    <h2>Weather Clock Web Terminal</h2>

    <div class="controls">
        <div class="control-group">
            <label class="switch">
                <input type="checkbox" id="autoScrollToggle" checked>
                <span class="slider"></span>
            </label>
            <span>Auto Scroll</span>
        </div>

        <div class="control-group">
            <label for="fontSize">Font Size:</label>
            <select id="fontSize">
                <option value="12">12px</option>
                <option value="14" selected>14px</option>
                <option value="16">16px</option>
                <option value="18">18px</option>
                <option value="20">20px</option>
            </select>
        </div>

        <div class="control-group">
            <label class="switch">
                <input type="checkbox" id="themeToggle">
                <span class="slider"></span>
            </label>
            <span>Dark Theme</span>
        </div>
    </div>

    <input type="text" id="searchInput" placeholder="Search logs...">

    <div class="terminal" id="terminal"></div>

    <div class="input-container">
        <input type="text" id="cmdInput" placeholder="Enter command...">
    </div>

    <div class="button-container">
        <button id="sendBtn" onclick="sendCommand()">Send</button>
        <button id="clearBtn" onclick="clearTerminal()">Clear</button>
        <button id="disconnectBtn" onclick="toggleConnection()">Disconnect</button>
        <button id="logViewerBtn" onclick="openLogViewer()">Log Viewer</button>
    </div>

    <script>
        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        var terminal = document.getElementById('terminal');
        var cmdInput = document.getElementById('cmdInput');
        var commandHistory = [];
        var historyIndex = -1;
        var originalContent = '';
        var autoScroll = true;

        // 설정 저장/복원
        function saveSettings() {
            const settings = {
                fontSize: document.getElementById('fontSize').value,
                autoScroll: document.getElementById('autoScrollToggle').checked,
                darkTheme: document.getElementById('themeToggle').checked
            };
            localStorage.setItem('terminalSettings', JSON.stringify(settings));
        }

        function loadSettings() {
            const settings = JSON.parse(localStorage.getItem('terminalSettings') || '{}');

            if (settings.fontSize) {
                document.getElementById('fontSize').value = settings.fontSize;
                terminal.style.fontSize = settings.fontSize + 'px';
            }

            if (settings.autoScroll !== undefined) {
                document.getElementById('autoScrollToggle').checked = settings.autoScroll;
                autoScroll = settings.autoScroll;
            }

            if (settings.darkTheme !== undefined) {
                document.getElementById('themeToggle').checked = settings.darkTheme;
                document.body.setAttribute('data-theme', settings.darkTheme ? 'dark' : 'light');
            }
        }

        // 이벤트 리스너 설정
        document.getElementById('fontSize').addEventListener('change', function(e) {
            terminal.style.fontSize = e.target.value + 'px';
            saveSettings();
        });

        document.getElementById('autoScrollToggle').addEventListener('change', function(e) {
            autoScroll = e.target.checked;
            saveSettings();
            if (autoScroll) {
                terminal.scrollTop = terminal.scrollHeight;
            }
        });

        document.getElementById('themeToggle').addEventListener('change', function(e) {
            document.body.setAttribute('data-theme', e.target.checked ? 'dark' : 'light');
            saveSettings();
        });

        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            websocket = new WebSocket(gateway);
            websocket.onopen = onOpen;
            websocket.onclose = onClose;
            websocket.onmessage = onMessage;
        }

        function onOpen(event) {
            console.log('Connection opened');
            document.getElementById('websocket-status').innerHTML = 'WebSocket: Connected';
            document.getElementById('disconnectBtn').innerHTML = 'Disconnect';
        }

        function onClose(event) {
            console.log('Connection closed');
            document.getElementById('websocket-status').innerHTML = 'WebSocket: Disconnected';
            document.getElementById('disconnectBtn').innerHTML = 'Connect';
            setTimeout(initWebSocket, 2000);
        }

        function onMessage(event) {
            try {
                const data = JSON.parse(event.data);
                if (data.type === 'wifi_status') {
                    document.getElementById('wifi-signal').textContent =
                        `Signal: ${data.rssi}dBm`;
                    return;
                }
            } catch {
                const formattedData = event.data.replace(
                    /\[(\d{2}:\d{2}:\d{2})\]/g,
                    '<span class="timestamp">[$1]</span>'
                );
                terminal.innerHTML += formattedData;
                originalContent = terminal.innerHTML;
                localStorage.setItem('terminalContent', terminal.innerHTML);

                if (autoScroll) {
                    terminal.scrollTop = terminal.scrollHeight;
                }
            }
        }

        function sendCommand() {
            var cmd = cmdInput.value;
            if (cmd) {
                websocket.send(cmd);
                commandHistory.unshift(cmd);
                historyIndex = -1;
                cmdInput.value = '';
            }
        }

        function clearTerminal() {
            terminal.innerHTML = '';
            originalContent = '';
            localStorage.removeItem('terminalContent');
        }

        function toggleConnection() {
            if (websocket.readyState === WebSocket.OPEN) {
                websocket.close();
            } else {
                initWebSocket();
            }
        }

        function openLogViewer() {
            window.location.href = '/log';
        }

        document.getElementById('searchInput').addEventListener('input', function(e) {
            const searchText = e.target.value.toLowerCase();

            if (searchText === '') {
                terminal.innerHTML = originalContent;
                return;
            }

            const lines = originalContent.split('\n');
            const filteredLines = lines.filter(line =>
                line.toLowerCase().includes(searchText)
            );

            if (filteredLines.length > 0) {
                terminal.innerHTML = filteredLines.join('\n');
            } else {
                terminal.innerHTML = '<div class="no-results">No matching logs found</div>';
            }
        });

        cmdInput.addEventListener('keydown', function(event) {
            if (event.key === 'Enter') {
                sendCommand();
            } else if (event.key === 'ArrowUp') {
                if (historyIndex < commandHistory.length - 1) {
                    historyIndex++;
                    cmdInput.value = commandHistory[historyIndex];
                }
                event.preventDefault();
            } else if (event.key === 'ArrowDown') {
                if (historyIndex > -1) {
                    historyIndex--;
                    cmdInput.value = historyIndex >= 0 ? commandHistory[historyIndex] : '';
                }
                event.preventDefault();
            }
        });

        window.onload = function() {
            initWebSocket();
            loadSettings();
            cmdInput.focus();

            const savedContent = localStorage.getItem('terminalContent');
            if (savedContent) {
                terminal.innerHTML = savedContent;
                originalContent = savedContent;
                if (autoScroll) {
                    terminal.scrollTop = terminal.scrollHeight;
                }
            }
        };
    </script>
</body>
</html>
)rawliteral";


// 로그 페이지 HTML
const char log_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>Weather Clock Log Viewer</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        :root {
            --bg-color: #f0f0f0;
            --text-color: #333;
            --terminal-bg: #000;
            --terminal-text: #00ff00;
            --timestamp-color: #00BFFF;
        }

        [data-theme="dark"] {
            --bg-color: #1a1a1a;
            --text-color: #fff;
            --terminal-bg: #000;
            --terminal-text: #00ff00;
            --timestamp-color: #00BFFF;
        }

        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background: var(--bg-color);
            color: var(--text-color);
            transition: background-color 0.3s, color 0.3s;
        }
        .status-bar {
            background: #333;
            color: white;
            padding: 8px 15px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-radius: 4px;
        }
        .log-container {
            background-color: var(--terminal-bg);
            color: var(--terminal-text);
            font-family: monospace;
            padding: 10px;
            height: 400px;
            overflow-y: scroll;
            margin: 10px 0;
            border-radius: 4px;
            white-space: pre-wrap;
            font-size: 14px;
        }
        .timestamp {
            color: var(--timestamp-color);
        }
        .button-container {
            display: flex;
            gap: 10px;
            margin-top: 10px;
            flex-wrap: wrap;
        }
        .controls {
            display: flex;
            gap: 10px;
            align-items: center;
            margin: 10px 0;
            flex-wrap: wrap;
        }
        .control-group {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        button {
            padding: 8px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            color: white;
            min-width: 100px;
        }
        #backBtn { background-color: #607D8B; }
        #downloadBtn { background-color: #4CAF50; }
        #clearBtn { background-color: #f44336; }
        button:hover { opacity: 0.8; }
        .no-results {
            color: #888;
            text-align: center;
            padding: 20px;
            font-style: italic;
        }
        input[type="text"] {
            width: 100%;
            padding: 8px;
            margin: 5px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
            background: var(--bg-color);
            color: var(--text-color);
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 50px;
            height: 24px;
        }
        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 24px;
        }
        .slider:before {
            position: absolute;
            content: "";
            height: 16px;
            width: 16px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        input:checked + .slider {
            background-color: #2196F3;
        }
        input:checked + .slider:before {
            transform: translateX(26px);
        }
        select {
            padding: 4px 8px;
            border-radius: 4px;
            background: var(--bg-color);
            color: var(--text-color);
        }
    </style>
</head>
<body>
    <div class="status-bar">
        <h2 style="margin: 0;">Weather Clock Log Viewer</h2>
        <div id="log-count">Logs: 0</div>
    </div>

    <div class="controls">
        <div class="control-group">
            <label class="switch">
                <input type="checkbox" id="autoScrollToggle" checked>
                <span class="slider"></span>
            </label>
            <span>Auto Scroll</span>
        </div>

        <div class="control-group">
            <label for="fontSize">Font Size:</label>
            <select id="fontSize">
                <option value="12">12px</option>
                <option value="14" selected>14px</option>
                <option value="16">16px</option>
                <option value="18">18px</option>
                <option value="20">20px</option>
            </select>
        </div>

        <div class="control-group">
            <label class="switch">
                <input type="checkbox" id="themeToggle">
                <span class="slider"></span>
            </label>
            <span>Dark Theme</span>
        </div>
    </div>

    <input type="text" id="searchInput" placeholder="Search logs...">

    <div class="log-container" id="logViewer"></div>

    <div class="button-container">
        <button id="backBtn" onclick="goBack()">Back</button>
        <button id="downloadBtn" onclick="downloadLogs()">Download</button>
        <button id="clearBtn" onclick="clearLogs()">Clear</button>
    </div>

    <script>
        var logViewer = document.getElementById('logViewer');
        var originalContent = '';
        var autoScroll = true;

        function loadSettings() {
            const settings = JSON.parse(localStorage.getItem('terminalSettings') || '{}');

            if (settings.fontSize) {
                document.getElementById('fontSize').value = settings.fontSize;
                logViewer.style.fontSize = settings.fontSize + 'px';
            }

            if (settings.autoScroll !== undefined) {
                document.getElementById('autoScrollToggle').checked = settings.autoScroll;
                autoScroll = settings.autoScroll;
            }

            if (settings.darkTheme !== undefined) {
                document.getElementById('themeToggle').checked = settings.darkTheme;
                document.body.setAttribute('data-theme', settings.darkTheme ? 'dark' : 'light');
            }
        }

        function saveSettings() {
            const settings = {
                fontSize: document.getElementById('fontSize').value,
                autoScroll: document.getElementById('autoScrollToggle').checked,
                darkTheme: document.getElementById('themeToggle').checked
            };
            localStorage.setItem('terminalSettings', JSON.stringify(settings));
        }

        document.getElementById('fontSize').addEventListener('change', function(e) {
            logViewer.style.fontSize = e.target.value + 'px';
            saveSettings();
        });

        document.getElementById('autoScrollToggle').addEventListener('change', function(e) {
            autoScroll = e.target.checked;
            saveSettings();
            if (autoScroll) {
                logViewer.scrollTop = logViewer.scrollHeight;
            }
        });

        document.getElementById('themeToggle').addEventListener('change', function(e) {
            document.body.setAttribute('data-theme', e.target.checked ? 'dark' : 'light');
            saveSettings();
        });

        function goBack() {
            window.location.href = '/';
        }

        function downloadLogs() {
            window.location.href = '/downloadlogs';
        }

        function clearLogs() {
            fetch('/clearlogs')
                .then(response => {
                    if(response.ok) {
                        logViewer.innerHTML = '';
                        originalContent = '';
                        document.getElementById('log-count').textContent = 'Logs: 0';
                    }
                });
        }

        function loadLogs() {
            fetch('/getlogs')
                .then(response => response.text())
                .then(data => {
                    const formattedData = data.replace(
                        /\[(\d{2}:\d{2}:\d{2})\]/g,
                        '<span class="timestamp">[$1]</span>'
                    );
                    logViewer.innerHTML = formattedData;
                    originalContent = formattedData;
                    const logCount = data.split('\n').filter(line => line.trim()).length;
                    document.getElementById('log-count').textContent = `Logs: ${logCount}`;

                    if (autoScroll) {
                        logViewer.scrollTop = logViewer.scrollHeight;
                    }
                });
        }

        document.getElementById('searchInput').addEventListener('input', function(e) {
            const searchText = e.target.value.toLowerCase();

            if (searchText === '') {
                logViewer.innerHTML = originalContent;
                return;
            }

            const lines = originalContent.split('\n');
            const filteredLines = lines.filter(line =>
                line.toLowerCase().includes(searchText)
            );

            if (filteredLines.length > 0) {
                logViewer.innerHTML = filteredLines.join('\n');
            } else {
                logViewer.innerHTML = '<div class="no-results">No matching logs found</div>';
            }
        });

        // 페이지 로드 시 설정 로드 및 로그 불러오기
        window.onload = function() {
            loadSettings();
            loadLogs();
        };

        // 주기적으로 로그 업데이트 (10초마다)
        setInterval(loadLogs, 10000);
    </script>
</body>
</html>
)rawliteral";
#endif //WEBSOCKET_H
