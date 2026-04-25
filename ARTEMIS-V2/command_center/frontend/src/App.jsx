import React, { useEffect, useState, useRef } from 'react';
import { io } from 'socket.io-client';
import RadarArray from './components/RadarArray';
import OpticalFeed from './components/OpticalFeed';
import TelemetryPanel from './components/TelemetryPanel';

function App() {
  const [socket, setSocket] = useState(null);
  const [criticalAlert, setCriticalAlert] = useState(null);
  const [telemetry, setTelemetry] = useState({ angle: 0, distance: 0 });
  const [objectTemp, setObjectTemp] = useState(24.2);
  const alertTimeoutRef = useRef(null);

  useEffect(() => {
    const newSocket = io('http://localhost:3001');
    setSocket(newSocket);

    newSocket.on('telemetry_update', (data) => {
      setTelemetry(data);
    });

    newSocket.on('critical_alert', (data) => {
      setTelemetry(data);
      setCriticalAlert(data);
      // Spike object temp to a random human body temperature
      setObjectTemp((Math.random() * (37.8 - 37.1) + 37.1).toFixed(1));

      // Trigger browser-native siren beep
      try {
        const AudioContext = window.AudioContext || window.webkitAudioContext;
        if (AudioContext) {
          const ctx = new AudioContext();
          const osc = ctx.createOscillator();
          const gainNode = ctx.createGain();
          
          osc.type = 'square';
          // High low high low siren sound
          osc.frequency.setValueAtTime(800, ctx.currentTime);
          osc.frequency.setValueAtTime(1200, ctx.currentTime + 0.2);
          osc.frequency.setValueAtTime(800, ctx.currentTime + 0.4);
          osc.frequency.setValueAtTime(1200, ctx.currentTime + 0.6);
          
          gainNode.gain.setValueAtTime(0.1, ctx.currentTime);
          gainNode.gain.exponentialRampToValueAtTime(0.01, ctx.currentTime + 1.0);
          
          osc.connect(gainNode);
          gainNode.connect(ctx.destination);
          
          osc.start();
          osc.stop(ctx.currentTime + 1.0);
        }
      } catch (e) {
        console.error("Audio beep failed:", e);
      }

      // Automatically reset after 3 seconds
      if (alertTimeoutRef.current) clearTimeout(alertTimeoutRef.current);
      alertTimeoutRef.current = setTimeout(() => {
        setCriticalAlert(null);
        setObjectTemp(24.2);
      }, 3000);
    });

    return () => {
      newSocket.close();
      if (alertTimeoutRef.current) clearTimeout(alertTimeoutRef.current);
    };
  }, []);

  return (
    <div className={`w-screen h-screen font-mono flex flex-col p-4 gap-4 overflow-hidden transition-colors duration-300 ${criticalAlert ? 'bg-red-950 text-white' : 'bg-black text-[#00FF41]'}`}>
      {/* Header */}
      <header className={`border-b-2 pb-2 ${criticalAlert ? 'border-red-500' : 'border-[#00FF41]'}`}>
        <h1 className="text-2xl font-black tracking-widest uppercase">
          A.R.T.E.M.I.S. COMMAND CENTER v2.0
        </h1>
        {criticalAlert && <div className="text-red-500 font-bold animate-pulse mt-1">SYSTEM COMPROMISED - THREAT DETECTED</div>}
      </header>

      {/* Main Grid */}
      <div className="flex-1 grid grid-cols-12 gap-6 h-full min-h-0">
        {/* Left Panel: 180-Degree Radar */}
        <div className="col-span-4 h-full">
          <RadarArray telemetry={telemetry} criticalAlert={criticalAlert} />
        </div>

        {/* Center Panel: Optical Feed */}
        <div className="col-span-4 h-full relative">
          <OpticalFeed criticalAlert={criticalAlert} />
        </div>

        {/* Right Panel: Telemetry & Thermal */}
        <div className="col-span-4 h-full">
          <TelemetryPanel telemetry={telemetry} objectTemp={objectTemp} criticalAlert={criticalAlert} />
        </div>
      </div>
    </div>
  );
}

export default App;
