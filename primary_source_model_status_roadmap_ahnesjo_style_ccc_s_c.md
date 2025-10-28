# Primary Source Model — Status & Roadmap (Ahnesjö‑style CCC/S‑C)

**Context.** We’ve implemented the core *primary (direct) fluence* driver for a CCC/S‑C dose engine on the isocentre plane. This note captures what’s done, what’s missing relative to the classic Ahnesjö model family, and a minimal commissioning roadmap.

---

## 1) What’s implemented (✅)
- **Finite focal spot → Gaussian blur (separable).**
  - Isotropic 2‑D Gaussian with σ commissioned at isocentre; convolution applied to a geometric aperture mask.
- **Geometric aperture at isocentre.**
  - Jaws rectangle rasterized with anti‑aliasing (supersampling); no transmission outside field.
- **Fluence normalization.**
  - CAX normalized to 1.0 for a reference open field (e.g., 10×10 cm²).
- **HDF5 I/O.**
  - `/plane_isocenter/primary/fluence_primary [ny×nx]` (float32) with `filled` + `normalized` attrs; grid axes `x_mm`, `y_mm` (1‑D).

**Operational outcome:** A clean incident *primary* fluence map on the isocentre plane suitable to drive TERMA in a CCC/S‑C chain.

---

## 2) Gaps vs. Ahnesjö‑style primary modelling (to add) (⚠️)
1. **Off‑axis attenuation / quality through the head**
   - Effective transmission *T_p(δ)* and optional hardening term *k_p(δ)* to capture flattening‑filter + heel effects (intensity + spectrum vs off‑axis angle δ).
2. **Jaw/MLC transmission (primary leakage)**
   - Small constant transmission through closed leaves/jaws; can be modelled as a thin primary component or moved to extra‑focal.
3. **Source anisotropy / ellipticity**
   - Elliptic Gaussian (σ_x, σ_y) to match measured penumbrae in in‑plane/cross‑plane.
4. **Distance dependence / planes beyond isocentre**
   - Inverse‑square scaling and ray‑path transmission if scoring on arbitrary planes (e.g., EPID) or during TERMA ray‑marching.
5. **Measured‑data fit for primary OAR**
   - Fit *T_p(δ)* to in‑air open‑field profiles (e.g., 40×40 cm²) to reproduce a “primary‑only” off‑axis ratio.
6. **Integration with extra‑focal sources** *(not primary per se, but required for total incident fluence)*
   - Flattening‑filter scatter (eye‑view release) and collimator scatter components.

---

## 3) Minimal roadmap to complete the “primary”
**P1. Elliptic focal spot.**
- Replace σ → (σ_x, σ_y). Commission from small‑field (e.g., 5×5 cm²) in‑air penumbrae in both axes.

**P2. Off‑axis primary transmission.**
- Introduce *T_p(δ)* as a smooth function (polynomial/spline or 2‑D map vs radius). Fit to in‑air open‑field OAR (primary‑dominated region) and keep amplitude separate from extra‑focal fit.

**P3. Optional jaw/leaf transmission.**
- Add a constant transmission fraction multiplied by the geometrically blocked area (or attribute it to extra‑focal, but track it explicitly).

**P4. Generalize scoring plane.**
- Allow arbitrary z‑planes with inverse‑square and head‑path attenuation using *T_p* along each ray; retain isocentre as the commissioning plane.

---

## 4) Commissioning data (minimum)
- **Penumbra (small‑field) in air:** Orthogonal profiles (e.g., 5×5 cm²) at isocentre → fit σ_x, σ_y.
- **Open‑field OAR in air:** Large field (e.g., 40×40 cm²) → fit *T_p(δ)* (primary off‑axis shape).
- **Transmission:** Jaw/leaf leakage (closed field measurement) → optional constant.

---

## 5) File/parameter hooks (current & proposed)
- **Current HDF5:**
  - `/plane_isocenter/grid/{x_mm,y_mm}`; attrs: `spacing_mm`, `origin_mm`.
  - `/plane_isocenter/primary/fluence_primary` (float32) with attrs `filled`, `normalized`.
  - `/plane_isocenter/primary` attrs: `sigma_mm`, `fwhm_mm`, `z_source_mm`, `focal_spot_model="gaussian"`.
- **Proposed additions:**
  - `/plane_isocenter/primary` attrs: `sigma_x_mm`, `sigma_y_mm` (if elliptic), `primary_Tp_model` (e.g., "poly"), `Tp_coeffs[...]` (or a 1‑D dataset `/primary/Tp_of_delta`).
  - Optional dataset `/primary/head_transmission_map` if using a 2‑D effective thickness.

---

## 6) Integration notes (with CCC/S‑C)
- Use primary fluence as the TERMA driver; if *k_p(δ)* (quality) is modelled, select/warp kernel parameters vs δ.
- Keep extra‑focal sources separate during commissioning; sum into `/total/fluence_total` only after each component is validated.

---

### References (indicative)
- Ahnesjö A. et al., model‑based photon dose calculation (convolution/superposition; primary vs extra‑focal source decomposition).
- Follow‑on implementations in commercial CCC engines (elliptic primary, off‑axis transmission/quality curves).

> This note is intentionally light and implementation‑oriented. When you’re ready, we can expand each bullet into code tasks (headers, data flow, math) and add explicit bibliographic citations.

