# PCFX4PS3 (Pre-Alpha / WIP)

**PCFX4PS3** es un proyecto experimental de código abierto cuyo objetivo es portar un emulador independiente y optimizado de **NEC PC-FX** (consola de 32 bits lanzada en 1994) de forma nativa a **PlayStation 3** utilizando el SDK abierto **PSL1GHT**.

> **Estado del Proyecto:** `Pre-Alpha / En Desarrollo Activo`  
> Actualmente nos encontramos en la fase de integración estructural y adaptación de arquitectura (Big-Endian / Cell PPU + RSX). Aún no es una versión jugable final.

---

## Meta del Proyecto
Hasta ahora, la única forma de ejecutar títulos de NEC PC-FX en PlayStation 3 ha sido a través de núcleos pesados de Mednafen/RetroArch que sufren graves caídas de velocidad y cortes de sonido. **PCFX4PS3** busca resolver esto combinando:
1. Un **núcleo de emulación ligero en C puro** (`pcfxemu`), evitando la sobrecarga de memoria y dependencias de C++ en el procesador principal (PPU).
2. Un **frontend nativo de PS3** de bajo consumo con aceleración de video por hardware **RSX** y audio **Cell** de baja latencia a 48 kHz.
3. Desarrollo iterativo asistido por IA (*vibe coding*) enfocado en optimizar la decodificación de video FMV (chips KING / RAINBOW) y el procesador NEC V810 para el hardware de PS3.

---

## Arquitectura y Cómo lo Estamos Haciendo
El proyecto fusiona dos piezas clave en una estructura limpia y modular:
* **`ps3/` (Frontend y Drivers Nativos):** Interfaz estilo XMB con soporte para DualShock 3, escalado RSX, guardado rápido con miniaturas y gestor de archivos en `/dev_hdd0` y USB.
* **`core_pcfx/` (Núcleo Emulador en C Puro):** Motor de emulación extraído de `pcfxemu` (CPU NEC V810, chips HuC6270/HuC6261/HuC6272, sonido ADPCM/CDDA y lector CD-ROM) conectado mediante una capa puente (`pcfx_headless.h`).

---

## Progreso Actual
- [x] **Fase 1:** Configuración del entorno `ps3toolchain` + `PSL1GHT` en Ubuntu (WSL) y creación del repositorio limpio.
- [x] **Fase 2:** Integración de la base estructural del frontend de PS3 y aislamiento del núcleo en C puro (`core_pcfx/`).
- [ ] **Fase 3 (En curso):** Adaptación del `Makefile` y creación del puente `ps3_pcfx_core.c` para compilar la primera prueba de menús (`out/PCFX4PS3.pkg`) en PS3 real (HEN/CFW).
- [ ] **Fase 4:** Carga de BIOS (`pcfx.rom`), adaptación Big-Endian y ejecución de pruebas de imagen y sonido.
- [ ] **Fase 5:** Optimización de velocidad FMV con RSX y soporte completo para imágenes `.cue`/`.bin` e `.iso`.

---

## Créditos y Agradecimientos
Este proyecto es posible gracias al trabajo abierto de desarrolladores e ingenieros de la escena retro y de PlayStation 3:

* **[theheroGAC](https://github.com):** Por su excelente trabajo en **PCE4PS3**, del cual utilizamos la base del frontend XMB, los drivers de video RSX, el motor de audio Cell a 48 kHz y los scripts de empaquetado para PSL1GHT.
* **[gameblabla](https://github.com):** Por reescribir y optimizar **pcfxemu** en C puro con soporte *headless* y mejoras de rendimiento para plataformas de recursos limitados.
* **Equipo de Mednafen & Beetle PC-FX:** Por la ingeniería inversa, precisión de hardware y documentación de la arquitectura NEC PC-FX.
* **Comunidad PS3Dev / PSL1GHT:** Por mantener vivo el SDK abierto para PlayStation 3.

---

## Licencia
Este proyecto se distribuye bajo la licencia **GNU General Public License (GPL)**, respetando las licencias originales de los proyectos base utilizados.
