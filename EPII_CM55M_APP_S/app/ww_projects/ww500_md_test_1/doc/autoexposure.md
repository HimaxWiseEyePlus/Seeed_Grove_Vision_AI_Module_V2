## Notes on Automatic Exposure (AE).

The `common_config.h` file includes this:

```
#define SENSOR_AE_STABLE_CNT		10
```

The "AE" in the setting "SENSOR_AE_STABLE_CNT" implies an automatic exposure function. I asked Chat GPT this:

ChatGPT says:

I am using the Himax HM0360. do you know anything specific about its AE operation?

Yes! The **Himax HM0360** has a built-in **Automatic Exposure (AE) control**, which is designed for ultra-low-power applications. Here's how AE typically operates on this sensor:

---

### **HM0360 Automatic Exposure (AE) Behavior**
1. **Power-On / Reset Behavior**
   - The sensor starts with **default exposure settings**.
   - AE is typically **enabled by default**, but you can override it via registers.

2. **AE Algorithm Adjustments**
   - The HM0360 analyzes scene brightness and adjusts:
     - **Integration time (shutter speed)**
     - **Analog gain**
     - **Digital gain** (if necessary)
   - Adjustments happen frame by frame, converging to a stable exposure over time.

3. **AE Fine-Tuning**
   - You can configure AE parameters via **I2C registers**:
     - **AE speed**: Controls how fast exposure changes.
     - **AE target brightness**: Defines the desired scene brightness.
     - **AE max/min limits**: Restricts how much gain or integration time can change.
   - Some implementations allow tuning AE for **flicker rejection** (e.g., 50Hz/60Hz indoor lighting).

4. **Low-Light & High-Light Adaptation**
   - In **low light**, AE increases **gain** and **integration time**.
   - In **bright conditions**, AE reduces these to prevent overexposure.

5. **Continuous Adjustment**
   - AE runs continuously and adapts to **dynamic lighting changes**.
   - Some applications may disable AE after stabilization for **consistent** exposure.

---

### **Customizing AE on the HM0360**
- **I2C Registers**: AE settings are controlled via **Himax’s AE control registers**.
- **Manual Override**: You can disable AE and manually set exposure if needed.
- **AE Locking**: Some applications lock AE after initial adjustment for **stable imaging**.

Would you like help with register settings or tuning AE behavior for your specific use case? 🚀