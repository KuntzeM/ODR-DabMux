<!DOCTYPE html>
<html><head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>ODR-DabMux Configuration</title>
	<link rel="stylesheet" href="static/style.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="static/jquery-1.10.2.min.js"></script>
    <script type="text/javascript" src="static/script.js"></script>
</head>
<body>
    <h1>Configuration for {{version}}</h1>

    <ul id="info-nav">
        <li><a href="#general">General Options</a></li>
        <li><a href="#servicelist">Services</a></li>
        <li><a href="#subchannels">Subchannels</a></li>
        <li><a href="#components">Components</a></li>
        <li><a href="#rcmodules">RC Modules</a></li>
    </ul>

    <div id="info">
        <div id="general"><div class="cadre">
            <h2>General Multiplex Options</h2>
            <ul>
                <li>Number of frames to encode: {{g.nbframes}}</li>
                <li>Statistics server port: {{g.statsserverport}}</li>
                <li>Write SCCA field: {{g.writescca}}</li>
                <li>Write TIST timestamp: {{g.tist}}</li>
                <li>DAB Mode: {{g.dabmode}}</li>
                <li>Log to syslog: {{g.syslog}}</li>
            </ul>
        </div></div>
        <div id="servicelist"><div class="cadre">
            <h2>Services</h2>
            <ul>
                % for s in services:
                    <li>{{s.name}}: <i>{{s.label}} ({{s.shortlabel}})</i> &mdash; id = {{s.id}}</li>
                % end
            </ul>
        </div></div>
        <div id="subchannels"><div class="cadre">
            <h2>Subchannels</h2>
            <ul>
                % for s in subchannels:
                    <li>{{s.name}}: <i>{{s.type}}</i> &mdash; {{s.inputfile}}; {{s.bitrate}}kbps</li>
                % end
            </ul>
        </div></div>
        <div id="components"><div class="cadre">
            <h2>Components</h2>
            <ul>
                % for s in components:
                    <li>{{s.name}}: <i>{{s.label}} ({{s.shortlabel}})</i> &mdash; service {{s.service}}; subchannel {{s.subchannel}}; figtype {{s.figtype}}</li>
                % end
            </ul>
        </div></div>
        <div id="rcmodules"><div class="cadre">
            <h2>RC Modules</h2>
            <ul>
                % for m in rcmodules:
                    <li>{{m.name}}
                        <ul>
                            % for p in m.parameters:
                                <li><a href="/rc/{{m.name}}/{{p.param}}">{{p.param}}</a> : {{p.value}}</li>
                            % end
                        </ul>
                    </li>
                % end
            </ul>
        </div></div>
    </div>
</body>
</html>

