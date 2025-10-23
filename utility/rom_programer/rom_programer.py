import tkinter as tk
from tkinter import ttk

class EEPROM_GUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("LYF-CH347 EEPROM編輯器 V1.0")
        self.geometry("800x600")
        self.minsize(800, 600)

        # --- 1. 選單欄 ---
        menubar = tk.Menu(self)
        file_menu = tk.Menu(menubar, tearoff=0)
        file_menu.add_command(label="開啟")
        file_menu.add_separator()
        file_menu.add_command(label="離開", command=self.quit)
        menubar.add_cascade(label="文件(F)", menu=file_menu)

        edit_menu = tk.Menu(menubar, tearoff=0)
        edit_menu.add_command(label="編輯操作(B)")
        menubar.add_cascade(label="編輯操作(B)", menu=edit_menu)

        help_menu = tk.Menu(menubar, tearoff=0)
        help_menu.add_command(label="關於")
        menubar.add_cascade(label="關於(I)", menu=help_menu)
        self.config(menu=menubar)

        # --- 2. 工具欄 ---
        toolbar = ttk.Frame(self, padding=3)
        toolbar.pack(side="top", fill="x")

        for name in ["開啟", "保存", "燒錄", "擦除", "校驗", "關於"]:
            ttk.Button(toolbar, text=name).pack(side="left", padx=2)

        # --- 主體區域 ---
        main_frame = ttk.Frame(self, padding=3)
        main_frame.pack(fill="both", expand=True)

        # 左側框架 (3 + 4)
        left_frame = ttk.Frame(main_frame)
        left_frame.pack(side="left", fill="y", padx=3)

        # --- 3. 芯片顯示區域 ---
        chip_frame = ttk.LabelFrame(left_frame, text="芯片顯示區域", padding=5)
        chip_frame.pack(fill="x", pady=3)

        ttk.Label(chip_frame, text="類型:").grid(row=0, column=0, sticky="w")
        ttk.Combobox(chip_frame, values=["I2C_24_EEPROM", "SPI_Flash"]).grid(row=0, column=1, sticky="ew")

        ttk.Label(chip_frame, text="廠商:").grid(row=1, column=0, sticky="w")
        ttk.Combobox(chip_frame, values=["_24Cxx", "_25Cxx"]).grid(row=1, column=1, sticky="ew")

        ttk.Label(chip_frame, text="型號:").grid(row=2, column=0, sticky="w")
        ttk.Combobox(chip_frame, values=["AT24C01", "AT24C02"]).grid(row=2, column=1, sticky="ew")

        ttk.Button(chip_frame, text="檢測").grid(row=3, column=0, pady=3)
        ttk.Button(chip_frame, text="查找").grid(row=3, column=1, pady=3)

        ttk.Label(chip_frame, text="頁大小: 1B").grid(row=4, column=0, sticky="w")
        ttk.Label(chip_frame, text="總容量: 128B").grid(row=5, column=0, sticky="w")

        # --- 4. 電壓顯示區域 ---
        voltage_frame = ttk.LabelFrame(left_frame, text="電壓顯示區域", padding=5)
        voltage_frame.pack(fill="x", pady=3)

        ttk.Radiobutton(voltage_frame, text="1.8V").pack(anchor="w")
        ttk.Radiobutton(voltage_frame, text="3.3V").pack(anchor="w")

        # Placeholder 圖像
        placeholder = tk.Canvas(voltage_frame, width=120, height=100, bg="#dddddd")
        placeholder.create_text(60, 50, text="Image\nPlaceholder", fill="gray")
        placeholder.pack(pady=5)

        # --- 中央主區域 (6 + 7 + 8) ---
        right_frame = ttk.Frame(main_frame)
        right_frame.pack(side="right", fill="both", expand=True, padx=3)

        # --- 6. 數據顯示區域 ---
        data_frame = ttk.LabelFrame(right_frame, text="數據顯示區域", padding=5)
        data_frame.pack(fill="both", expand=True, pady=3)
        data_text = tk.Text(data_frame, wrap="none", height=10)
        data_text.insert("end", "00 01 02 03 04 05 06 07 08 09 ...\nFF FF FF FF FF ...")
        data_text.pack(fill="both", expand=True)

        # --- 7. 操作日誌顯示區域 ---
        log_frame = ttk.LabelFrame(right_frame, text="操作日誌顯示區域", padding=5)
        log_frame.pack(fill="x", pady=3)
        log_text = tk.Text(log_frame, height=5)
        log_text.insert("end", "2025-07-04 14:44:37 設備初始化完成\n")
        log_text.pack(fill="x", expand=True)

        # --- 8. 進度條 ---
        progress_frame = ttk.Frame(right_frame)
        progress_frame.pack(fill="x", pady=3)
        ttk.Label(progress_frame, text="進度:").pack(side="left")
        progress = ttk.Progressbar(progress_frame, value=40, maximum=100)
        progress.pack(side="left", fill="x", expand=True, padx=5)
        ttk.Label(progress_frame, text="40%").pack(side="left")

        # --- 5. 狀態欄 ---
        status_frame = ttk.Frame(self, relief="sunken", padding=(5, 2))
        status_frame.pack(side="bottom", fill="x")
        ttk.Label(status_frame, text="設備連線:").pack(side="left")
        ttk.Label(status_frame, text="unknown").pack(side="left", padx=(0, 10))
        ttk.Label(status_frame, text="設備版本: unknown").pack(side="left")

if __name__ == "__main__":
    app = EEPROM_GUI()
    app.mainloop()
