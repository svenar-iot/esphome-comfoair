![ComfoAir Controller Banner](images/banner.png)

[![GPLv3 License](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Get Hardware](https://img.shields.io/badge/Shop-Get_Plug--Play_Solution-8A2BE2)](https://shop.svenar.nl)

ESPHome-powered controller for Zehnder WHR / CA ventilation units that integrate seamlessly with Home Assistant.

---

![Features](images/features.png)

- **Comprehensive Climate Control**
  - 📊 Real-time fan speed monitoring
  - 🌡️ Multi-zone temperature tracking (Supply/Return/Exhaust/Outdoor)
  - ⚙️ Bypass & preheating status visualization
  - ↕️ Set ventilation level (Absent(Off) / Low / Medium / High)
  - 🌀 Set ventilation speed per level
  - ❄️ Frost protection monitoring
  - ⏲️ Operation hour tracking
  - 🏠 Home Assistant climate entity support
  - 🔄 Automatic firmware updates (Using ESPHome-builder)

---

![Supported Devices](images/supported_devices.png)

## ComfoAir Series
__Using:__ `comfoair_ca.yaml`
- Zehnder WHR 930 (aka ComfoAir 350 / CA350)
- Zehnder WHR 950 (aka ComfoAir 550 / CA550)
- Zehnder ComfoAir 160
- Zehnder ComfoAir 200
- Zehnder ComfoAir 500

## ComfoD Series
__Using:__ `comfoair_ca.yaml`
- Zehnder ComfoD 250
- Zehnder ComfoD 300
- Zehnder ComfoD 350
- Zehnder ComfoD 450
- Zehnder ComfoD 550

---

![Hardware](images/hardware.png)

Recommended hardware:
* ESP32 board
* MAX3232 board

For plug-and-play solutions, visit [shop.svenar.nl](https://shop.svenar.nl)

---

![Build & Deploy](images/build_and_deploy.png)

## Using ESPHome CLI

https://esphome.io/guides/cli.html

```sh
$ esphome build comfoair.yaml
$ esphome upload comfoair.yaml
```

## Using ESPHome-builder

https://esphome.io/guides/getting_started_hassio.html

1. Install the ESPHome-builder addin inside Home Assistant
2. Add the `comfoair.yaml` config
3. Build and upload

---

![Credits](images/credits.png)

This project builds upon the foundational work by [Wichers](https://github.com/wichers/esphome-comfoair).  

---

![License](images/license.png)

**License**: GNU General Public License v3.0 - See [LICENSE](LICENSE) for details.