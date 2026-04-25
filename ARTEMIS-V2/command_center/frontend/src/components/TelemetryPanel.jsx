import React from 'react';

const TelemetryPanel = ({ telemetry, objectTemp, criticalAlert }) => {
  return (
    <div className={`w-full h-full border p-4 flex flex-col gap-6 ${criticalAlert ? 'border-red-500 bg-red-950/30' : 'border-[#00FF41] bg-black'}`}>
      <h2 className={`text-sm tracking-widest uppercase border-b pb-2 ${criticalAlert ? 'border-red-500 text-red-500 font-bold' : 'border-[#00FF41] text-[#00FF41]'}`}>
        TELEMETRY & SENSORS
      </h2>

      {/* Target Coordinates */}
      <div className="space-y-2">
        <div className={`text-xs opacity-70 ${criticalAlert ? 'text-red-400' : 'text-[#00FF41]'}`}>TARGET TRACKING</div>
        <div className="grid grid-cols-2 gap-4">
          <div className={`border p-3 ${criticalAlert ? 'border-red-500 bg-red-900/50' : 'border-[#00FF41]/40 bg-black'}`}>
            <div className={`text-[10px] mb-1 ${criticalAlert ? 'text-red-400' : 'text-[#00FF41]'}`}>SWEEP ANGLE</div>
            <div className={`text-3xl font-black ${criticalAlert ? 'text-white' : 'text-[#00FF41]'}`}>
              {telemetry.angle.toFixed(0)}°
            </div>
          </div>
          <div className={`border p-3 ${criticalAlert ? 'border-red-500 bg-red-900/50' : 'border-[#00FF41]/40 bg-black'}`}>
            <div className={`text-[10px] mb-1 ${criticalAlert ? 'text-red-400' : 'text-[#00FF41]'}`}>DISTANCE</div>
            <div className={`text-3xl font-black ${criticalAlert ? 'text-white' : 'text-[#00FF41]'}`}>
              {telemetry.distance.toFixed(1)} <span className="text-sm">cm</span>
            </div>
          </div>
        </div>
      </div>

      {/* Thermal Data Fake-Out */}
      <div className="space-y-2">
        <div className={`text-xs opacity-70 ${criticalAlert ? 'text-red-400' : 'text-[#00FF41]'}`}>THERMAL SIGNATURE</div>
        <div className={`border p-4 flex justify-between items-center transition-colors duration-200 ${criticalAlert ? 'border-red-500 bg-red-600 shadow-[0_0_20px_#dc2626]' : 'border-[#00FF41]/40'}`}>
          <span className={`text-sm ${criticalAlert ? 'text-white font-bold' : 'text-[#00FF41]'}`}>OBJECT TEMP</span>
          <span className={`text-4xl font-black ${criticalAlert ? 'text-white animate-pulse' : 'text-[#00FF41]'}`}>
            {objectTemp} °C
          </span>
        </div>
        {criticalAlert && (
          <div className="text-red-400 text-xs mt-2 text-right animate-pulse font-bold">
            WARNING: ANOMALOUS HEAT SIGNATURE DETECTED
          </div>
        )}
      </div>

      {/* System Status */}
      <div className="mt-auto">
        <div className={`text-xs p-2 border font-bold ${criticalAlert ? 'border-red-500 text-white bg-red-700 animate-pulse' : 'border-[#00FF41]/40 text-[#00FF41]'}`}>
          SYS STATUS: {criticalAlert ? 'CRITICAL ALERT - SERVO LOCKED' : 'NOMINAL SWEEP'}
        </div>
      </div>
    </div>
  );
};

export default TelemetryPanel;
