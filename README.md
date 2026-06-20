# 🔋 Battery Management System (BMS) — Lithium 3S

Système de gestion de batterie lithium 3 cellules (3S) développé sur **STM32 Blue Pill** avec supervision en temps réel, protections actives, équilibrage passif et contrôle moteur via PWM.

![Schéma complet](images/schema_full.png)

---

## 📌 Fonctionnalités

- ✅ Surveillance en temps réel : tension par cellule, tension pack, courant
- ✅ Protections : surtension, sous-tension, surcourant
- ✅ Équilibrage cellulaire passif (Cell Balancing)
- ✅ Contrôle de vitesse moteur via PWM (TIM1)
- ✅ Affichage LCD I²C (tensions, courant, vitesse)
- ✅ Communication UART avec Arduino (commande de vitesse)
- ✅ Affichage 7 segments (vitesse côté Arduino)

---

## 🏗️ Architecture du système

Le système est composé de deux microcontrôleurs qui communiquent via UART :

```
[Arduino UNO]  ──UART──►  [STM32 Blue Pill]
  Potentiomètre               ADC (mesure cellules)
  Affichage 7 seg             PWM moteur
  Envoi vitesse               Équilibrage + protections
                              LCD I²C
```

---

## ⚡ Bloc BMS — Mesure & Protection & Balancing

![Bloc BMS](images/schema_bms_block.png)

### Mesure des tensions cellules
Les 3 cellules lithium sont mesurées via un pont diviseur de tension sur les canaux ADC (CH0, CH1, CH2) du STM32.

```c
bat_Voltage[0] = Read_ADC(ADC_CHANNEL_0) / coeff_Res[0];
bat_Voltage[1] = Read_ADC(ADC_CHANNEL_1) / coeff_Res[1] - bat_Voltage[0];
bat_Voltage[2] = Read_ADC(ADC_CHANNEL_2) / coeff_Res[2] - (bat_Voltage[0] + bat_Voltage[1]);
```

### Seuils de protection

| Paramètre | Valeur |
|---|---|
| Tension max cellule | 4.20 V |
| Seuil équilibrage | 4.00 V |
| Tension nominale | 3.70 V |
| Tension min cellule | 3.00 V |
| Tension critique | 2.80 V |
| Tension pack min | 9.00 V |

### Équilibrage passif (Cell Balancing)

Activé uniquement en charge. Si une cellule dépasse **4.00 V**, une résistance de décharge est activée via GPIO pour ramener sa tension au niveau des autres cellules.

```c
void balance_Cell() {
    if (charging == 1) {
        if (bat_Voltage[0] > V_CELL_BAL)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        // idem pour cellules 2 et 3...
    }
}
```

---

## Bloc STM32 + Affichage LCD

![STM32 et LCD](images/schema_stm_lcd.png)

Le STM32 Blue Pill est le cerveau du système :
- **ADC1** : lecture tension des 3 cellules + courant (ACS712 sur CH3)
- **TIM1 PWM** : contrôle vitesse moteur
- **I²C** : affichage LCD 16x2
- **UART** : réception commande vitesse depuis Arduino


---

##  Bloc Moteur — Contrôle PWM

![Moteur](images/schema_motor.png)

La vitesse du moteur est contrôlée par **PWM via TIM1** du STM32. La commande vient de l'Arduino (potentiomètre → valeur 0-50 → UART → STM32).

```c
void CMD_Vitesse() {
    if (total_Voltage >= V_PACK_MIN) {
        // calcul CCR selon vitesse reçue
        TIM1->CCR1 = ccr_value;
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    }
    // moteur arrêté si batterie faible
}
```

> Si la tension pack descend sous **9.00 V**, le moteur est coupé automatiquement pour protéger la batterie.

---

##  Bloc Arduino — Interface utilisateur

L'Arduino gère l'interface utilisateur :
- Lecture d'un **potentiomètre** (0–50) via ADC
- Affichage de la vitesse sur un **afficheur 7 segments multiplexé**
- Envoi de la valeur vitesse au STM32 via **UART SoftwareSerial**

```c
vitesse = map(adcValue, 0, 1023, 0, 50);
uart.write((uint8_t)vitesse);
```

---

##  Composants utilisés

| Composant | Rôle |
|---|---|
| STM32 Blue Pill (STM32F103C8) | Microcontrôleur principal |
| Arduino UNO | Interface utilisateur |
| ACS712 | Capteur de courant |
| LCD 16x2 I²C | Affichage données |
| Afficheur 7 segments | Affichage vitesse |
| MOSFETs / transistors | Équilibrage + protection |
| Potentiomètre | Commande vitesse |

---

##  Protocoles utilisés

| Protocole | Usage |
|---|---|
| ADC | Lecture tensions cellules et courant |
| I²C | Communication LCD |
| UART | Communication STM32 ↔ Arduino |
| PWM (TIM1) | Contrôle moteur |

---

## 📁 Structure du projet

```
BMS-Lithium-3S/
├── STM32/
│   └── main.c              # Code principal STM32
├── Arduino/
│   └── arduino_speed.ino   # Code Arduino interface
├── images/
│   └── schema_full.png     # Schéma Proteus complet
└── README.md
```

---

## 👤 Auteur

**Cheikh Samb** — Élève-ingénieur ENSA Marrakech   [LinkedIn](https://www.linkedin.com/in/cheikh-samb-a46a982a8/)
**Iruno  Lefort** — Élève-ingénieur ENSA Marrakech   [LinkedIn](https://www.linkedin.com/in/cheikh-samb-a46a982a8/)
**Bill-ELvis** — Élève-ingénieur ENSA Marrakech 
Systèmes Électroniques Embarqués et Commande des Systèmes   [LinkedIn](https://www.linkedin.com/in/cheikh-samb-a46a982a8/)

