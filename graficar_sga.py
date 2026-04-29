"""
Grafica los resultados del Algoritmo Genético Simple (SGA)
por generación a partir de la salida del programa C++.

INSTRUCCIONES:
1. Modifica main.cpp para imprimir datos CSV así:

   // Antes del for de corridas (una sola vez):
   cout << "GEN,CORRIDA,MEJOR" << endl;

   // Dentro del while de generaciones (después de ConvertirObjetivoAptitud):
   cout << numero_generacion << "," << corrida << "," << ag.ObtenerMejorObjetivo() << "\n";

2. Ejecuta tu programa y guarda la salida:
   ./sga -genes 30 -bits 16 -poblacion 50 -generaciones 100 \
         -sup 100 100 -inf -100 -100 > salida.csv 2>/dev/null

   (el 2>/dev/null elimina los mensajes de error/debug de stderr si los mandas ahí)

   O si todo va a stdout, filtra solo las líneas numéricas:
   ./sga ... | grep -E "^[0-9]" > salida.csv
   Y agrega manualmente la cabecera: GEN,CORRIDA,MEJOR

3. Ejecuta este script:
   python3 graficar_sga.py --archivo salida.csv --funcion "f4"
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import argparse
import sys
import os

# ── Argumentos de línea de comandos ─────────────────────────────────────────
parser = argparse.ArgumentParser(description="Grafica convergencia del SGA por generación")
parser.add_argument("--archivo", type=str, default="salida.csv",
                    help="Archivo CSV con columnas GEN,CORRIDA,MEJOR")
parser.add_argument("--funcion", type=str, default="f?",
                    help="Nombre de la función benchmark (ej: f4, f5, f12)")
parser.add_argument("--log", action="store_true",
                    help="Usar escala logarítmica en eje Y")
args = parser.parse_args()

# ── Cargar datos ─────────────────────────────────────────────────────────────
if not os.path.exists(args.archivo):
    print(f"[ERROR] No se encontró el archivo: {args.archivo}")
    print("Genera la salida con tu programa C++ primero.")
    sys.exit(1)

df = pd.read_csv(args.archivo, comment="#")  # ignora líneas "# MEAN=..."

# Verificar columnas
required = {"GEN", "CORRIDA", "MEJOR"}
if not required.issubset(df.columns):
    print(f"[ERROR] El CSV debe tener columnas: GEN, CORRIDA, MEJOR")
    print(f"Columnas encontradas: {list(df.columns)}")
    sys.exit(1)

# Forzar tipos numéricos (por si quedó alguna fila no numérica)
df = df[pd.to_numeric(df["GEN"], errors="coerce").notna()]
df["GEN"]    = df["GEN"].astype(int)
df["CORRIDA"]= df["CORRIDA"].astype(int)
df["MEJOR"]  = df["MEJOR"].astype(float)

print(f"Datos cargados: {len(df)} filas, "
      f"{df['CORRIDA'].nunique()} corridas, "
      f"{df['GEN'].nunique()} generaciones")

# ── Calcular estadísticas por generación ─────────────────────────────────────
stats = df.groupby("GEN")["MEJOR"].agg(
    media="mean",
    mediana="median",
    minimo="min",
    maximo="max",
    std="std",
    q25=lambda x: x.quantile(0.25),
    q75=lambda x: x.quantile(0.75),
).reset_index()

# ── Seleccionar corridas individuales para mostrar (máx 10) ──────────────────
corridas_ids = df["CORRIDA"].unique()
corridas_muestra = corridas_ids[:min(10, len(corridas_ids))]

# ── Figura principal: 2x2 ────────────────────────────────────────────────────
fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle(f"Convergencia del Algoritmo Genético Simple — Función {args.funcion}\n"
             f"({df['CORRIDA'].nunique()} ejecuciones, {df['GEN'].nunique()} generaciones)",
             fontsize=14, fontweight="bold", y=0.98)

COLOR_MEDIA   = "#E63946"
COLOR_MEJOR   = "#2A9D8F"
COLOR_BANDA   = "#457B9D"
COLOR_CORRIDA = "#A8DADC"

# ────────────────────────────────────────────────────────────────────────────
# Gráfica 1: Corridas individuales + media
# ────────────────────────────────────────────────────────────────────────────
ax1 = axes[0, 0]
for cid in corridas_muestra:
    subset = df[df["CORRIDA"] == cid].sort_values("GEN")
    ax1.plot(subset["GEN"], subset["MEJOR"],
             color=COLOR_CORRIDA, alpha=0.5, linewidth=0.8)

ax1.plot(stats["GEN"], stats["media"],
         color=COLOR_MEDIA, linewidth=2.2, label="Media (100 corridas)", zorder=5)

ax1.set_title("Corridas individuales + Media", fontsize=11)
ax1.set_xlabel("Generación")
ax1.set_ylabel("Mejor valor objetivo")
ax1.legend(fontsize=9)
ax1.grid(True, alpha=0.3)
if args.log:
    ax1.set_yscale("log")

# ────────────────────────────────────────────────────────────────────────────
# Gráfica 2: Media ± Desviación estándar (banda de confianza)
# ────────────────────────────────────────────────────────────────────────────
ax2 = axes[0, 1]
ax2.fill_between(stats["GEN"],
                 stats["media"] - stats["std"],
                 stats["media"] + stats["std"],
                 color=COLOR_BANDA, alpha=0.25, label="±1 Desv. Estándar")
ax2.plot(stats["GEN"], stats["media"],
         color=COLOR_MEDIA, linewidth=2.2, label="Media")
ax2.plot(stats["GEN"], stats["minimo"],
         color=COLOR_MEJOR, linewidth=1.5, linestyle="--", label="Mínimo global")

ax2.set_title("Media ± Desviación Estándar", fontsize=11)
ax2.set_xlabel("Generación")
ax2.set_ylabel("Mejor valor objetivo")
ax2.legend(fontsize=9)
ax2.grid(True, alpha=0.3)
if args.log:
    ax2.set_yscale("log")

# ────────────────────────────────────────────────────────────────────────────
# Gráfica 3: Boxplot por generación (cada N generaciones)
# ────────────────────────────────────────────────────────────────────────────
ax3 = axes[1, 0]
max_gen = df["GEN"].max()
# Seleccionar ~10 puntos de generaciones para el boxplot
step = max(1, max_gen // 10)
gens_box = list(range(step, max_gen + 1, step))

data_box = [df[df["GEN"] == g]["MEJOR"].values for g in gens_box]
bp = ax3.boxplot(data_box, positions=gens_box, widths=step * 0.6,
                 patch_artist=True, showfliers=False,
                 boxprops=dict(facecolor=COLOR_BANDA, alpha=0.5),
                 medianprops=dict(color=COLOR_MEDIA, linewidth=2))

ax3.set_title("Distribución por generación (Boxplot)", fontsize=11)
ax3.set_xlabel("Generación")
ax3.set_ylabel("Mejor valor objetivo")
ax3.grid(True, alpha=0.3, axis="y")
if args.log:
    ax3.set_yscale("log")

# ────────────────────────────────────────────────────────────────────────────
# Gráfica 4: Histograma del mejor resultado final de cada corrida
# ────────────────────────────────────────────────────────────────────────────
ax4 = axes[1, 1]
# Obtener el mejor valor de la última generación de cada corrida
ultima_gen = df["GEN"].max()
mejores_finales = df[df["GEN"] == ultima_gen]["MEJOR"].values

media_final = mejores_finales.mean()
std_final   = mejores_finales.std()

ax4.hist(mejores_finales, bins=20, color=COLOR_BANDA, edgecolor="white",
         alpha=0.85, label="Distribución final")
ax4.axvline(media_final, color=COLOR_MEDIA, linewidth=2.2,
            label=f"Media = {media_final:.4e}")
ax4.axvline(media_final - std_final, color=COLOR_MEDIA,
            linewidth=1.2, linestyle="--", alpha=0.7)
ax4.axvline(media_final + std_final, color=COLOR_MEDIA,
            linewidth=1.2, linestyle="--", alpha=0.7,
            label=f"StdDev = {std_final:.4e}")

ax4.set_title("Histograma — Mejor valor final (100 corridas)", fontsize=11)
ax4.set_xlabel("Mejor valor objetivo (generación final)")
ax4.set_ylabel("Frecuencia")
ax4.legend(fontsize=9)
ax4.grid(True, alpha=0.3, axis="y")

# ── Resumen en consola ────────────────────────────────────────────────────────
print("\n=== RESUMEN ESTADÍSTICO (generación final) ===")
print(f"  Media (Mean):       {media_final:.6e}")
print(f"  Desv. Estándar:     {std_final:.6e}")
print(f"  Mínimo encontrado:  {mejores_finales.min():.6e}")
print(f"  Máximo encontrado:  {mejores_finales.max():.6e}")

# ── Guardar figura ────────────────────────────────────────────────────────────
plt.tight_layout(rect=[0, 0, 1, 0.96])
nombre_salida = f"convergencia_{args.funcion}.png"
plt.savefig(nombre_salida, dpi=150, bbox_inches="tight")
print(f"\nGráfica guardada como: {nombre_salida}")
plt.show()