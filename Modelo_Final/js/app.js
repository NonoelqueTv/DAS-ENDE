// Modelo_Final\js\app.js
// TM + Arduino (Local) - app.js (VERSIÓN 4 CLASES)

const MODEL_URL    = "./tm_model/model.json";
const METADATA_URL = "./tm_model/metadata.json";

// CONFIGURACIÓN DE UMBRALES (4 Clases)
// El orden debe coincidir con el panel de Teachable Machine:
// [0] = Vale
// [1] = NADA (Antes era Benja) -> Activa Audio
// [2] = Avaro (Alvaro)
// [3] = Benja (Antes era NADA)
const classThresholds = [0.98, 0.98, 0.98, 0.98];

let model, webcam, maxPredictions, labelContainer;
let port, writer, lastClass = -1;
let running = false; 

async function connectSerial() {
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 115200 });
    writer = port.writable.getWriter();
    alert("Arduino conectado.");
  } catch (e) {
    console.error("Serial:", e);
    alert("No se pudo abrir el puerto serial.");
  }
}

async function init() {
  if (running) return;
  running = true;

  try {
    const [r1, r2] = await Promise.all([fetch(MODEL_URL), fetch(METADATA_URL)]);
    if (!r1.ok || !r2.ok) throw new Error("Faltan archivos del modelo en ./tm_model/");

    model = await tmImage.load(MODEL_URL, METADATA_URL);
    maxPredictions = model.getTotalClasses();

    webcam = new tmImage.Webcam(320, 240, true);
    await webcam.setup();
    await webcam.play();
    const wc = document.getElementById("webcam-container");
    wc.innerHTML = "";
    wc.appendChild(webcam.canvas);

    labelContainer = document.getElementById("label-container");
    labelContainer.innerHTML = "";
    for (let i = 0; i < maxPredictions; i++) {
      labelContainer.appendChild(document.createElement("div"));
    }

    window.requestAnimationFrame(loop);
  } catch (e) {
    console.error("Error:", e);
    alert("Error cargando modelo. Revisa la consola.");
    running = false;
  }
}

async function loop() {
  webcam.update();
  await predict();
  window.requestAnimationFrame(loop);
}

async function predict() {
  const pred = await model.predict(webcam.canvas);
  let bestIdx = 0, bestProb = 0;

  for (let i = 0; i < pred.length; i++) {
    const p = pred[i];
    labelContainer.childNodes[i].innerHTML = `${p.className}: ${p.probability.toFixed(2)}`;
    
    if (p.probability > bestProb) { 
        bestProb = p.probability; 
        bestIdx = i; 
    }
  }

  // LOGICA DE ENVIO
  if (bestProb > classThresholds[bestIdx] && bestIdx !== lastClass) {
    lastClass = bestIdx;
    
    if (writer) {
      const enc = new TextEncoder();
      await writer.write(enc.encode(`CLASE:${bestIdx}\n`));
      console.log(`Enviado: ${pred[bestIdx].className} (Clase ${bestIdx}) - Certeza: ${(bestProb*100).toFixed(1)}%`);
    }
  }
}

document.getElementById("btnStart").onclick = init;
document.getElementById("btnConnect").onclick = connectSerial;