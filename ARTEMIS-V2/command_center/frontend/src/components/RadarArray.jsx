import React, { useEffect, useRef } from 'react';

const RadarArray = ({ telemetry, criticalAlert }) => {
  const canvasRef = useRef(null);
  
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    
    // Resize to match container visually
    const { width, height } = canvas.getBoundingClientRect();
    canvas.width = width;
    canvas.height = height;
    
    // 180-degree radar configuration (center at bottom)
    const cx = width / 2;
    const cy = height - 20; 
    const maxRadius = Math.min(width / 2 - 20, height - 40);
    
    ctx.clearRect(0, 0, width, height);
    
    // Base colors
    const gridColor = criticalAlert ? '#7f1d1d' : '#004400';
    const mainColor = criticalAlert ? '#ef4444' : '#00FF41';
    
    // Draw semi-circle rings
    ctx.strokeStyle = gridColor;
    ctx.lineWidth = 1;
    for (let r = maxRadius / 5; r <= maxRadius; r += maxRadius / 5) {
      ctx.beginPath();
      ctx.arc(cx, cy, r, Math.PI, 0); // Math.PI to 0 represents the top half
      ctx.stroke();
    }
    
    // Draw angle dividers (0 to 180 degrees)
    for (let a = 0; a <= 180; a += 30) {
      // Map 0 -> left (Math.PI) to 180 -> right (0) or vice versa depending on servo orientation
      // Mathematically on canvas: Math.PI is left, 0 is right.
      // So (a + 180) * Math.PI / 180 goes from Math.PI -> 2Math.PI (which is 0). 
      const rad = Math.PI + (a * Math.PI / 180);
      ctx.beginPath();
      ctx.moveTo(cx, cy);
      ctx.lineTo(cx + maxRadius * Math.cos(rad), cy + maxRadius * Math.sin(rad));
      ctx.stroke();
    }
    
    // Draw sweeping arm based on telemetry.angle
    const angleRad = Math.PI + (telemetry.angle * Math.PI / 180);
    ctx.strokeStyle = mainColor;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(cx + maxRadius * Math.cos(angleRad), cy + maxRadius * Math.sin(angleRad));
    ctx.stroke();
    
    // Draw detection blip if distance is within range (0 to 50cm)
    if (telemetry.distance > 0 && telemetry.distance <= 50) {
      const distRatio = telemetry.distance / 50;
      const blipRadius = distRatio * maxRadius;
      const bx = cx + blipRadius * Math.cos(angleRad);
      const by = cy + blipRadius * Math.sin(angleRad);
      
      if (criticalAlert) {
        // Red flashing dot
        ctx.fillStyle = '#ef4444';
        ctx.beginPath();
        ctx.arc(bx, by, 8, 0, Math.PI * 2);
        ctx.fill();
        
        ctx.strokeStyle = '#ef4444';
        ctx.beginPath();
        ctx.arc(bx, by, 16, 0, Math.PI * 2);
        ctx.stroke();
      } else {
        // Normal green dot
        ctx.fillStyle = '#00FF41';
        ctx.beginPath();
        ctx.arc(bx, by, 4, 0, Math.PI * 2);
        ctx.fill();
      }
    }
    
  }, [telemetry, criticalAlert]);

  return (
    <div className={`w-full h-full border ${criticalAlert ? 'border-red-500 bg-red-950/50' : 'border-[#00FF41] bg-[#001100]'} relative p-2 flex flex-col`}>
      <h2 className={`text-sm tracking-widest uppercase mb-2 ${criticalAlert ? 'text-red-500 font-bold' : 'text-[#00FF41]'}`}>
        TACTICAL RADAR ARRAY [180°]
      </h2>
      <div className="flex-1 w-full relative">
        <canvas ref={canvasRef} className="absolute inset-0 w-full h-full" />
      </div>
    </div>
  );
};

export default RadarArray;
