// TM + Arduino (Local) - app.js (FIX)
// Requiere abrir con Live Server y tener ./tm_model/{model.json,metadata.json,weights.bin}

const MODEL_URL    = "./tm_model/model.json";
const METADATA_URL = "./tm_model/metadata.json";

let model, webcam, maxPredictions, labelContainer;
let port, writer, lastClass = -1;
let running = false; // evita múltiples inits

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
  if (running) return;        // no iniciar dos veces
  running = true;

  try {
    // Verificación previa (ayuda a diagnosticar 404)
    const [r1, r2] = await Promise.all([fetch(MODEL_URL), fetch(METADATA_URL)]);
    console.log("model.json", r1.status, "metadata.json", r2.status);
    if (!r1.ok || !r2.ok) throw new Error("Faltan archivos del modelo en ./tm_model/");

    model = await tmImage.load(MODEL_URL, METADATA_URL);
    maxPredictions = model.getTotalClasses();

    // Webcam
    webcam = new tmImage.Webcam(320, 240, true);
    await webcam.setup();
    await webcam.play();
    const wc = document.getElementById("webcam-container");
    wc.innerHTML = "";                   // limpia si había algo
    wc.appendChild(webcam.canvas);

    // Etiquetas
    labelContainer = document.getElementById("label-container");
    labelContainer.innerHTML = "";
    for (let i = 0; i < maxPredictions; i++) {
      labelContainer.appendChild(document.createElement("div"));
    }

    window.requestAnimationFrame(loop);
  } catch (e) {
    console.error("Error cargando el modelo:", e);
    alert("No se pudo cargar el modelo. ¿Copiaste model.json, metadata.json y weights.bin a ./tm_model/?");
    running = false;
  }
}

async function loop() {
  webcam.update();
  await predict();
  window.requestAnimationFrame(loop);
}

async function predict() {
  const pred = await model.predict(webcam.canvas); // Array {className, probability}
  let bestIdx = 0, bestProb = 0;

  for (let i = 0; i < pred.length; i++) {
    const p = pred[i];
    labelContainer.childNodes[i].innerHTML = `${p.className}: ${p.probability.toFixed(2)}`;
    if (p.probability > bestProb) { bestProb = p.probability; bestIdx = i; }
  }

  if (bestProb > 0.80 && bestIdx !== lastClass) {
    lastClass = bestIdx;
    if (writer) {
      const enc = new TextEncoder();
      await writer.write(enc.encode(`CLASE:${bestIdx}\n`));
    }
  }
}

document.getElementById("btnStart").onclick = init;
document.getElementById("btnConnect").onclick = connectSerial;
