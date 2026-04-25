import React from 'react';

const OpticalFeed = ({ criticalAlert }) => {
  return (
    <div className={`w-full h-full border ${criticalAlert ? 'border-red-500' : 'border-[#00FF41]'} relative flex flex-col bg-black overflow-hidden`}>
      <h2 className={`text-sm tracking-widest uppercase p-2 border-b z-10 ${criticalAlert ? 'border-red-500 text-red-500 bg-red-950' : 'border-[#00FF41] text-[#00FF41] bg-black'}`}>
        OPTICAL SENSOR FEED
      </h2>
      
      <div className="flex-1 relative w-full h-full bg-[#0a0a0a] flex items-center justify-center">
        {/* MJPEG Stream */}
        <img 
          src="http://10.42.191.83:81/stream" 
          alt="ESP32-CAM Feed" 
          className={`w-full h-full object-cover ${criticalAlert ? 'opacity-50 grayscale mix-blend-screen' : 'opacity-80 grayscale'}`}
          onError={(e) => { 
            e.target.style.display = 'none'; 
            e.target.parentElement.classList.add('bg-[url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSI0MCIgaGVpZ2h0PSI0MCI+PHRleHQgeT0iMjAiIGZpbGw9IiMwMEZGNDEiIGZvbnQtZmFtaWx5PSJtb25vc3BhY2UiIGZvbnQtc2l6ZT0iMTAiPk5PIFNJR05BTDwvdGV4dD48L3N2Zz4=")]');
            e.target.parentElement.classList.add('bg-center', 'bg-no-repeat');
          }}
        />
        
        {/* Tactical Crosshair Overlay */}
        <div className="absolute inset-0 pointer-events-none flex items-center justify-center">
          <svg width="200" height="200" viewBox="0 0 100 100" className={`${criticalAlert ? 'stroke-red-500 animate-pulse scale-110' : 'stroke-[#00FF41]'} transition-transform duration-200`}>
            <circle cx="50" cy="50" r="40" fill="none" strokeWidth="1" strokeDasharray="4 4" />
            <circle cx="50" cy="50" r="10" fill="none" strokeWidth="1" />
            <line x1="50" y1="0" x2="50" y2="35" strokeWidth="2" />
            <line x1="50" y1="65" x2="50" y2="100" strokeWidth="2" />
            <line x1="0" y1="50" x2="35" y2="50" strokeWidth="2" />
            <line x1="65" y1="50" x2="100" y2="50" strokeWidth="2" />
            {criticalAlert && <circle cx="50" cy="50" r="2" fill="#ef4444" stroke="none" />}
          </svg>
        </div>

        {/* SOS Banner */}
        {criticalAlert && (
          <div className="absolute inset-x-0 top-1/3 bg-red-600 text-white text-center py-4 text-2xl lg:text-3xl font-black uppercase tracking-widest shadow-[0_0_30px_rgba(220,38,38,0.8)] animate-bounce border-y-4 border-red-800 z-50">
            SOS: HUMAN TARGET LOCKED
          </div>
        )}
      </div>
    </div>
  );
};

export default OpticalFeed;
