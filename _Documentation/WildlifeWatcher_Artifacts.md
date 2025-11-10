# Wildlife Watcher Artifact Management

This document outlines the various digital artifacts that comprise the Wildlife Watcher ecosystem, how they are managed, and how they interact with the mobile application and backend services.

## 1. Artifact Background and Context

The Wildlife Watcher device relies on three primary types of digital artifacts to function: Firmware, Machine Learning (ML) Models, and Configuration settings.
 
*   **Firmware**: The device's functionality is split between two microprocessors, each with its own firmware image.
    *   **BLE Processor Firmware**: Manages Bluetooth Low Energy (BLE) communication with the mobile app, LoRaWAN connectivity, and orchestrates the overall device state.
    *   **Himax AI Processor Firmware**: Controls the camera sensor, runs the machine learning model, and manages the SD card.
*   **Machine Learning (ML) Artifacts**: The AI capabilities are defined by the ML model and its accompanying documentation.
    *   **ML Model**: The trained neural network model that identifies species in images.
    *   **Model Manuscript & Labels**: A document describing the model, its training data, and a `labels.txt` file that maps the model's output to species names.
*   **Configuration File (`CONFIG.TXT`)**: A source file that defines default device parameters like `Delay before DPD` and `Num pics when triggered`.
 
## 2. Current State
 
This section describes how artifacts are currently managed and deployed. The process is largely manual.
 
### 2.1. Configuration (`CONFIG.TXT`) Handling
The `CONFIG.TXT` file is **not transferred to the device as a separate file**. Instead, its values are compiled directly into the Himax AI processor firmware image (`.img`) during the build process.
 
*   **Location**: `EPII_CM55M_APP_S/app/ww_projects/ww500_md/CONFIG/CONFIG.TXT`
*   **Interaction**: These compiled-in values serve as the device's default settings. The mobile app can override them at runtime by sending specific BLE commands (e.g., `set heartbeat`, `AI capture`).
 
### 2.2. ML Model Zoo
The Wildlife Watcher Model Zoo is currently a **model formatting and conversion tool**. It contains scripts and utilities to prepare pre-trained models (e.g., from Edge Impulse) into the specific format required by the Himax AI processor. It is a developer-facing tool, not a user-facing model repository.
 
### 2.3. Versioning and Storage
*   **Versioning**: Firmware versions are tracked manually using Git tags in their respective GitHub repositories. ML models are versioned based on the scheme used in Edge Impulse.
*   **Storage**: There is no central automated registry. Artifacts are built locally by developers and manually placed in the sd card or transferred via computer connection.
 
## 3. Target State and Automation
 
The goal is to create a fully automated, single source of truth for all artifacts, managed through Supabase and the mobile app. This will streamline development, reduce errors, and enable new user-facing features.
 
### 3.1. Artifact Management Pipeline
A CI/CD pipeline will be implemented to automate the versioning and uploading of all artifacts to Supabase.
 
1.  **Trigger**: A new release is created in a firmware or model GitHub repository (e.g., a new Git tag is pushed).
2.  **Build**: A GitHub Actions workflow checks out the code, builds the firmware (`.zip` for BLE, `.img` for Himax), or packages the ML model files.
3.  **Upload to Supabase Storage**: The built artifact is uploaded to a dedicated bucket in Supabase Storage (e.g., `firmware-releases`, `ml-models`).
4.  **Update Database Registry**: The workflow then calls a Supabase Edge Function to update the `firmware` or `ai_models` table with metadata for the new version, including its name, version number, storage path, and file size. The `is_active` flag is set to `true`.
5.  **Mobile App Discovery**: The mobile app can then query these tables to discover and download the latest artifacts for offline use.
 
### 3.2. ML Model Zoo Integration
The Model Zoo will be integrated with Supabase to create a user-friendly platform for model management.
 
*   **User Uploads**: **Organisation Administrators** will be able to upload their own custom-trained models through a web interface (the Admin Portal).
*   **Automated Conversion**: The upload process will trigger a backend service that uses the Model Zoo's conversion scripts to validate and format the model.
*   **Registry Update**: Once processed, the model will be added to the `ai_models` registry in Supabase and made available to that specific organization.
*   **In-App Selection**: **Project Admins** can then select these custom models for their projects directly within the mobile app.
 
This automated system will provide a scalable and secure way to manage all software and models, ensuring that devices in the field can be reliably updated with the correct artifacts.

