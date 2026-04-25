import React, { useEffect, useState, useRef } from 'react';

const EventLog = ({ socket }) => {
  const [logs, setLogs] = useState([]);
  const bottomRef = useRef(null);

  useEffect(() => {
    if (!socket) return;
    
    socket.on('EVENT_LOG', (log) => {
      setLogs(prev => {
        const newLogs = [...prev, log];
        // Keep max 50 logs to prevent memory leak
        if (newLogs.length > 50) return newLogs.slice(newLogs.length - 50);
        return newLogs;
      });
    });

    return () => socket.off('EVENT_LOG');
  }, [socket]);

  useEffect(() => {
    // Auto-scroll to bottom
    if (bottomRef.current) {
      bottomRef.current.scrollIntoView({ behavior: 'smooth' });
    }
  }, [logs]);

  return (
    <div className="h-full bg-[#000000] border border-hud-green flex flex-col p-2 crt">
      <div className="text-[10px] text-hud-green opacity-70 mb-2 font-bold tracking-widest border-b border-[#00FF4140] pb-1">
        TACTICAL EVENT LOG // TTY1
      </div>
      
      <div className="flex-1 overflow-y-auto space-y-1 font-mono text-xs p-1">
        {logs.map((log, index) => {
          const timeString = new Date(log.timestamp).toLocaleTimeString('en-US', { hour12: false, fractionalSecondDigits: 3 });
          
          let colorClass = "text-hud-green";
          if (log.type === "alert") colorClass = "text-hud-red font-bold";
          if (log.type === "warn") colorClass = "text-yellow-400";

          return (
            <div key={index} className={`flex gap-2 ${colorClass}`}>
              <span className="opacity-50">[{timeString}]</span>
              <span>{log.message}</span>
            </div>
          );
        })}
        <div ref={bottomRef} />
      </div>
    </div>
  );
};

export default EventLog;
