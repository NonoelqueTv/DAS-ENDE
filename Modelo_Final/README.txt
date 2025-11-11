TM + Arduino (Local) - Proyecto listo
============================================

Cómo usar:
1) En Teachable Machine -> Exportar -> TensorFlow.js -> **Descargar**.
2) Copiá dentro de `tm_model/`:
   - model.json
   - metadata.json
   - weights.bin
3) En VSCode -> Open with **Live Server** sobre `index.html` (http://127.0.0.1:5500).
4) Botón **Start** -> permití la cámara.
5) Botón **Conectar Arduino** -> elegí el puerto (115200).

Salida Serial cuando cambia la clase con p > 0.80:
  CLASE:0
  CLASE:1
  CLASE:2

Si querés enviar los **nombres** en vez del índice, avisá y te agrego el mapeo.
