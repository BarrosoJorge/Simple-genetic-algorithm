from PIL import Image, ImageDraw, ImageFont
import csv, os, math

CSV_PATH = "resultados_circulos.csv"
DATA_DIR = "data"
OUT_DIR  = "resultados"
os.makedirs(OUT_DIR, exist_ok=True)

IMG_MAP = {
    "C01.bmp": "4_img.bmp",
    "C02.bmp": "5_img.bmp",
    "C03.bmp": "6_img.bmp",
    "C04.bmp": "7_img.bmp",
    "C05.bmp": "8_img.bmp",
    "auto.bmp": "3_img.bmp",
    "lamp.bmp": "9_img.bmp",
}

# --- leer CSV ---
datos = {}
with open(CSV_PATH) as f:
    for row in csv.DictReader(f):
        n = row["imagen"]
        if n not in datos:
            datos[n] = []
        datos[n].append({
            "cx":    float(row["cx"]),
            "cy":    float(row["cy"]),
            "r":     float(row["r"]),
            "score": float(row["score"]),
        })

def stats(vals):
    n   = len(vals)
    mu  = sum(vals) / n
    std = math.sqrt(sum((v - mu)**2 for v in vals) / n)
    return mu, std

def draw_text(draw, x, y, text, color=(255, 0, 0)):
    # dibuja texto con sombra para legibilidad
    draw.text((x+1, y+1), text, fill=(0, 0, 0))
    draw.text((x,   y  ), text, fill=color)

# --- procesar cada imagen ---
for nombre, runs in datos.items():
    src_path = os.path.join(DATA_DIR, IMG_MAP[nombre])
    original = Image.open(src_path).convert("RGB")
    W, H     = original.size

    # mejor run
    best = max(runs, key=lambda r: r["score"])

    # estadísticas
    mcx, scx = stats([r["cx"]    for r in runs])
    mcy, scy = stats([r["cy"]    for r in runs])
    mr,  sr  = stats([r["r"]     for r in runs])
    ms       = sum(r["score"] for r in runs) / len(runs)

    # imagen con círculo del mejor run
    # BMP almacena filas de abajo hacia arriba → invertir cy para Pillow
    overlay = original.copy()
    draw    = ImageDraw.Draw(overlay)
    cx, cy, r = best["cx"], best["cy"], best["r"]
    cy_draw = H - cy
    draw.ellipse([cx-r, cy_draw-r, cx+r, cy_draw+r], outline=(255, 0, 0), width=2)
    # marca el centro
    draw.ellipse([cx-3, cy_draw-3, cx+3, cy_draw+3], outline=(255, 0, 0), fill=(255, 0, 0))

    # panel de texto (debajo de la imagen overlay)
    PAD  = 8
    LINE = 16
    ROWS = 5
    txt_h = PAD * 2 + LINE * ROWS
    panel = Image.new("RGB", (W, txt_h), (30, 30, 30))
    td    = ImageDraw.Draw(panel)
    tx, ty = PAD, PAD
    td.text((tx, ty),           f"{nombre}",                              fill=(255,255,100))
    td.text((tx, ty + LINE),    f"mejor:  cx={cx:.1f}  cy={cy:.1f}  r={r:.1f}  score={best['score']:.2f}", fill=(255,255,255))
    td.text((tx, ty + LINE*2),  f"cx:  {mcx:.1f} ± {scx:.1f}",           fill=(200,200,200))
    td.text((tx, ty + LINE*3),  f"cy:  {mcy:.1f} ± {scy:.1f}",           fill=(200,200,200))
    td.text((tx, ty + LINE*4),  f"r:   {mr:.1f}  ± {sr:.1f}   score_prom={ms:.2f}", fill=(200,200,200))

    # composición: izquierda=original, derecha=overlay+panel
    GAP   = 4
    right_h = H + GAP + txt_h
    canvas  = Image.new("RGB", (W * 2 + GAP, max(H, right_h)), (80, 80, 80))
    canvas.paste(original, (0, 0))
    canvas.paste(overlay,  (W + GAP, 0))
    canvas.paste(panel,    (W + GAP, H + GAP))

    # etiquetas de columna
    cd = ImageDraw.Draw(canvas)
    cd.text((4,       2), "ORIGINAL",        fill=(255, 255, 0))
    cd.text((W+GAP+4, 2), "MEJOR RUN (rojo)", fill=(255, 255, 0))

    out_path = os.path.join(OUT_DIR, nombre.replace(".bmp", "_overlay.png"))
    canvas.save(out_path)
    print(f"[OK] {out_path}")
    print(f"     mejor: cx={cx:.1f} cy={cy:.1f} r={r:.1f} score={best['score']:.2f}")
    print(f"     media: cx={mcx:.1f}±{scx:.1f}  cy={mcy:.1f}±{scy:.1f}  r={mr:.1f}±{sr:.1f}")
    print()
