#  Ashfall: Night of the Dead

> **A Top-Down Zombie Shooter demonstrating advanced Object-Oriented Programming (OOP) concepts.**

##  Project Overview
**Ashfall** is a C++ based survival shooter where players must survive endless waves of zombies. This project was built to demonstrate the practical application of OOP principles in game development, moving beyond theory into functional design patterns.

---

##  Key Features

* **Day-Night Cycle:** Dynamic lighting and visibility changes; survival becomes harder at night.
* **In-Game Shop:** Purchase upgrades and items to survive longer.
* **Endless Waves:** Difficulty scales with every wave of zombies.
* **Smooth Camera:** Logic to fluidly track player movement.
* **Game States:** Full support for Main Menu, Pause, and Game Over states.
* **Particle System:** A particle system to make the game more dynamic. Spray of blood and sticking blood on the ground.
* **Background Music:** The game plays thrilling background music.

---

##  OOPs Concepts Implemented

This project strictly adheres to OOP standards to ensure modularity and memory safety.

### 1. Encapsulation
* **Concept:** Bundling data and methods that work on that data within one unit.
* **Implementation:** Classes like `Player.h` manage their own private data (e.g., `health`, `ammo count`). Access is restricted to public methods, preventing external classes from directly modifying internal state.

### 2. Abstraction
* **Concept:** Hiding complex implementation details behind simple interfaces.
* **Implementation:** The `AudioManager.h` class handles low-level sound logic. The main engine doesn't need to know *how* sound is played, it only interacts with the public `play()` methods.

### 3. Composition (Has-A Relationship)
* **Concept:** Building complex objects from simpler ones, preferred over deep Inheritance trees.
* **Implementation:** The project relies on Composition. For example, the `Shop` class "has a" list of `ShopItems`, rather than inheriting from an item class.

### 4. Static Members (Flyweight Pattern)
* **Concept:** Sharing data among all instances of a class to save memory.
* **Implementation:** We use static variables for textures. We do not load the same zombie texture 100 times for 100 zombies; they all share one static reference in memory.

### 5. Memory Management (RAII & Smart Pointers)
* **Concept:** Resource Acquisition Is Initialization.
* **Implementation:** Manual `new` and `delete` are avoided.
    * **Code:** `std::vector<std::unique_ptr<ShopItem>> items;`
    * **Benefit:** `std::unique_ptr` ensures that when the Shop is destroyed, all item memory is automatically freed, preventing memory leaks.

### 6. Enums (Scoped Enumerations)
* **Concept:** Defining custom types for state management for better readability and type safety.
* **Implementation:**
    * **Zombie States:** `enum zombieType {normal = 0, sprinter = 1, tank = 2};`
    * **Game Flow:** `enum class GameState { MainMenu, InGame, GameOver };`

### 7. STL (Standard Template Library) Integration
* **Concept:** Efficient management of dynamic object collections.
* **Implementation:**
    * **Containers:** `std::vector` is used extensively for Zombies, Bullets, and Landmines.
    * **Algorithms:** `std::remove_if` is used in `Player.cpp` to efficiently clean up dead entities (like used bullets) from the update loop.

---

## Screenshots
**Main Menu**
<img width="1602" height="939" alt="image" src="https://github.com/user-attachments/assets/f255098f-8b97-4d4b-be23-335189ba1b33" />
---

**In Game Day**
<img width="1600" height="904" alt="image" src="https://github.com/user-attachments/assets/e4d3cc54-4825-49c0-9bf6-be4d1572ae40" />
---

**In Game Night**
<img width="1586" height="899" alt="image" src="https://github.com/user-attachments/assets/40b0179a-224c-44aa-9a80-121bfe2cef32" />

---
**In Game Shop**
<img width="1603" height="940" alt="image" src="https://github.com/user-attachments/assets/9ae45898-4041-4df4-80eb-e3a2c8c1af96" />
---

**Death Screen**
<img width="1404" height="790" alt="image" src="https://github.com/user-attachments/assets/853792c8-d0cc-43f3-9634-877bc7871e14" />
---

**Pause Menu**
<img width="1604" height="938" alt="image" src="https://github.com/user-attachments/assets/fafdadb9-5692-4dea-920e-f007c70b3669" />

