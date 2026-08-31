(() => {
  'use strict';

  const languageStorageKey = 'stepRgbLanguage';
  const supportedLanguages = ['tr', 'en', 'de', 'es', 'ar'];
  const rows = [
    ['Dil', 'Language', 'Sprache', 'Idioma', 'اللغة'],
    ['Ayarlar', 'Settings', 'Einstellungen', 'Ajustes', 'الإعدادات'],
    ['Geri', 'Back', 'Zurück', 'Volver', 'رجوع'],
    ['Yazılım v', 'Software v', 'Software v', 'Software v', 'البرنامج v'],

    ['STEP-RGB Panel Editörü', 'STEP-RGB Panel Editor', 'STEP-RGB Panel-Editor', 'Editor de panel STEP-RGB', 'محرر لوحة STEP-RGB'],
    ['Panel İçerik Editörü', 'Panel Content Editor', 'Panel-Inhaltseditor', 'Editor de contenido del panel', 'محرر محتوى اللوحة'],
    ['Metin, saat, tarih ve sıcaklığı panel üzerinde konumlandırın', 'Position text, time, date, and temperature on the panel', 'Text, Uhrzeit, Datum und Temperatur auf dem Panel positionieren', 'Coloque texto, hora, fecha y temperatura en el panel', 'ضع النص والوقت والتاريخ ودرجة الحرارة على اللوحة'],
    ['RGB Panel Önizleme', 'RGB Panel Preview', 'RGB-Panel-Vorschau', 'Vista previa del panel RGB', 'معاينة لوحة RGB'],
    ['Bir panel seçin, öğe ekleyin ve sürükleyerek taşıyın', 'Select a panel, add an item, and drag it into position', 'Panel auswählen, Element hinzufügen und per Ziehen positionieren', 'Seleccione un panel, añada un elemento y arrástrelo a su posición', 'اختر لوحة وأضف عنصراً ثم اسحبه إلى موضعه'],
    ['Panel bilgisi yükleniyor…', 'Loading panel information…', 'Panelinformationen werden geladen…', 'Cargando información del panel…', 'جارٍ تحميل معلومات اللوحة…'],
    ['Sürüklenebilir panel önizlemesi', 'Draggable panel preview', 'Ziehbare Panel-Vorschau', 'Vista previa del panel arrastrable', 'معاينة لوحة قابلة للسحب'],
    ['İpucu:', 'Tip:', 'Tipp:', 'Consejo:', 'تلميح:'],
    ['Öğeyi parmağınızla veya fareyle sürükleyin.', 'Drag the item with your finger or mouse.', 'Element mit Finger oder Maus ziehen.', 'Arrastre el elemento con el dedo o el ratón.', 'اسحب العنصر بإصبعك أو بالماوس.'],
    ['İçerik yükleniyor…', 'Loading content…', 'Inhalt wird geladen…', 'Cargando contenido…', 'جارٍ تحميل المحتوى…'],
    ['Panele Uygula', 'Apply to Panel', 'Auf Panel anwenden', 'Aplicar al panel', 'تطبيق على اللوحة'],
    ['Kutu Ayarları', 'Box Settings', 'Box-Einstellungen', 'Ajustes del cuadro', 'إعدادات المربع'],
    ['Seçilen öğenin görünümü, kaynağı ve hareketi', 'Appearance, source, and movement of the selected item', 'Darstellung, Quelle und Bewegung des ausgewählten Elements', 'Apariencia, fuente y movimiento del elemento seleccionado', 'مظهر العنصر المحدد ومصدره وحركته'],
    ['Düzenlemek için panelden bir öğe seçin veya yeni TextBox/ValueBox ekleyin.', 'Select an item on the panel to edit it, or add a new TextBox/ValueBox.', 'Zum Bearbeiten ein Element auf dem Panel auswählen oder eine neue TextBox/ValueBox hinzufügen.', 'Seleccione un elemento del panel para editarlo o añada un nuevo TextBox/ValueBox.', 'اختر عنصراً من اللوحة لتحريره أو أضف TextBox/ValueBox جديداً.'],
    ['Veri türü', 'Data type', 'Datentyp', 'Tipo de datos', 'نوع البيانات'],
    ['Yazı', 'Text', 'Text', 'Texto', 'نص'],
    ['Saat', 'Time', 'Uhrzeit', 'Hora', 'الوقت'],
    ['Tarih', 'Date', 'Datum', 'Fecha', 'التاريخ'],
    ['Sıcaklık', 'Temperature', 'Temperatur', 'Temperatura', 'درجة الحرارة'],
    ['İçerik döngüsü', 'Content cycle', 'Inhaltszyklus', 'Ciclo de contenido', 'دورة المحتوى'],
    ["Birden fazla bilgiyi aynı TextBox'ta sırayla gösterir", 'Shows multiple pieces of information in the same TextBox in sequence', 'Zeigt mehrere Informationen nacheinander in derselben TextBox an', 'Muestra varios datos secuencialmente en el mismo TextBox', 'يعرض عدة معلومات بالتتابع في TextBox نفسه'],
    ['İşaretlenen bilgiler Yazı → Saat → Tarih → Sıcaklık sırasıyla gösterilir. Kayar yazı açıksa içerik tam turunu tamamladıktan sonra değişir.', 'Selected information is shown in the order Text → Time → Date → Temperature. When scrolling is enabled, the content changes after one complete pass.', 'Ausgewählte Informationen werden in der Reihenfolge Text → Uhrzeit → Datum → Temperatur angezeigt. Bei aktiviertem Lauftext wechselt der Inhalt nach einem vollständigen Durchlauf.', 'La información seleccionada se muestra en el orden Texto → Hora → Fecha → Temperatura. Con desplazamiento activado, el contenido cambia tras una vuelta completa.', 'تُعرض المعلومات المحددة بالترتيب نص ← الوقت ← التاريخ ← درجة الحرارة. عند تفعيل التمرير يتغير المحتوى بعد اكتمال دورة كاملة.'],
    ['Değişim sıklığı (saniye)', 'Change interval (seconds)', 'Wechselintervall (Sekunden)', 'Intervalo de cambio (segundos)', 'فاصل التغيير (بالثواني)'],
    ['Yazdırılacak metin', 'Text to display', 'Anzuzeigender Text', 'Texto que se mostrará', 'النص المراد عرضه'],
    ['Metninizi yazın', 'Enter your text', 'Text eingeben', 'Escriba el texto', 'اكتب النص'],
    ['Sıcaklık gösterimi', 'Temperature display', 'Temperaturanzeige', 'Visualización de temperatura', 'عرض درجة الحرارة'],
    ['Float — bir ondalık (23.5°C)', 'Float — one decimal (23.5°C)', 'Float — eine Dezimalstelle (23,5°C)', 'Float — un decimal (23,5°C)', 'عشري — منزلة واحدة (23.5°C)'],
    ['Integer — tam sayı (23.5 → 24°C)', 'Integer — whole number (23.5 → 24°C)', 'Integer — Ganzzahl (23,5 → 24°C)', 'Integer — número entero (23,5 → 24°C)', 'صحيح — عدد كامل (23.5 ← 24°C)'],
    ['Integer seçiminde en yakın tam sayıya yuvarlanır; 0.5 ve üzeri bir üst değere çıkar.', 'Integer rounds to the nearest whole number; values of 0.5 and above round up.', 'Integer rundet auf die nächste Ganzzahl; ab 0,5 wird aufgerundet.', 'Integer redondea al entero más cercano; 0,5 o más se redondea hacia arriba.', 'يقرب الخيار الصحيح إلى أقرب عدد كامل؛ وتُقرب 0.5 فما فوق إلى الأعلى.'],
    ['Başlangıç değeri', 'Initial value', 'Startwert', 'Valor inicial', 'القيمة الابتدائية'],
    ['Cihaz her açıldığında ValueBox bu değerden başlar.', 'The ValueBox starts with this value whenever the device boots.', 'Die ValueBox startet bei jedem Gerätestart mit diesem Wert.', 'El ValueBox comienza con este valor cada vez que se inicia el dispositivo.', 'يبدأ ValueBox بهذه القيمة عند كل تشغيل للجهاز.'],
    ['Veri kaynağı', 'Data source', 'Datenquelle', 'Fuente de datos', 'مصدر البيانات'],
    ['Dijital giriş', 'Digital input', 'Digitaleingang', 'Entrada digital', 'دخل رقمي'],
    ['Slave ID: 1 • 9600 baud • 8N1 • Holding register. IEEE-754 float seçilen adresten başlayarak iki register kullanır (yüksek word önce).', 'Slave ID: 1 • 9600 baud • 8N1 • Holding register. IEEE-754 float uses two registers starting at the selected address (high word first).', 'Slave-ID: 1 • 9600 Baud • 8N1 • Holding-Register. IEEE-754 Float belegt ab der gewählten Adresse zwei Register (High-Word zuerst).', 'ID de esclavo: 1 • 9600 baudios • 8N1 • Holding register. IEEE-754 float usa dos registros desde la dirección seleccionada (palabra alta primero).', 'معرّف التابع: 1 • 9600 باود • 8N1 • سجل Holding. تستخدم قيمة IEEE-754 العشرية سجلين بدءاً من العنوان المحدد (الكلمة العليا أولاً).'],
    ['Register adresi', 'Register address', 'Registeradresse', 'Dirección del registro', 'عنوان السجل'],
    ['Değer tipi', 'Value type', 'Werttyp', 'Tipo de valor', 'نوع القيمة'],
    ['Sayaç yönü', 'Counter direction', 'Zählrichtung', 'Dirección del contador', 'اتجاه العداد'],
    ['Yukarı sayıcı', 'Count up', 'Aufwärts zählen', 'Cuenta ascendente', 'عد تصاعدي'],
    ['Aşağı sayıcı', 'Count down', 'Abwärts zählen', 'Cuenta descendente', 'عد تنازلي'],
    ['IO21 dahili pull-down ile LOW seviyesinde bekler. Debounce sonrasındaki LOW → HIGH yükselen kenarında değer artırılır veya azaltılır. Negatif değerler gösterilebilir.', 'IO21 waits at LOW with its internal pull-down. The value increments or decrements on a debounced LOW → HIGH rising edge. Negative values can be displayed.', 'IO21 wartet mit internem Pull-down auf LOW. Nach dem Entprellen wird der Wert bei einer LOW → HIGH-Flanke erhöht oder verringert. Negative Werte sind möglich.', 'IO21 espera en LOW con su pull-down interno. Tras el antirrebote, el valor aumenta o disminuye en el flanco LOW → HIGH. Se admiten valores negativos.', 'ينتظر IO21 عند المستوى LOW باستخدام مقاومة السحب لأسفل الداخلية. بعد إزالة الارتداد تزيد أو تنقص القيمة عند الحافة الصاعدة LOW ← HIGH. يمكن عرض القيم السالبة.'],
    ['Yazı tipi', 'Font', 'Schriftart', 'Fuente', 'الخط'],
    ['TextBox ve ValueBox için cihazdaki tüm Adafruit GFX fontları kullanılabilir.', 'All Adafruit GFX fonts on the device can be used for TextBox and ValueBox.', 'Für TextBox und ValueBox können alle Adafruit-GFX-Schriften auf dem Gerät verwendet werden.', 'Todos los tipos de letra Adafruit GFX del dispositivo pueden usarse en TextBox y ValueBox.', 'يمكن استخدام جميع خطوط Adafruit GFX الموجودة على الجهاز في TextBox وValueBox.'],
    ['Kutu yönü', 'Box orientation', 'Box-Ausrichtung', 'Orientación del cuadro', 'اتجاه المربع'],
    ['90° saat yönü', '90° clockwise', '90° im Uhrzeigersinn', '90° en sentido horario', '90° مع عقارب الساعة'],
    ['270° saat yönü', '270° clockwise', '270° im Uhrzeigersinn', '270° en sentido horario', '270° مع عقارب الساعة'],
    ['Renk', 'Color', 'Farbe', 'Color', 'اللون'],
    ['Kayar yazı', 'Scrolling text', 'Lauftext', 'Texto desplazable', 'نص متحرك'],
    ['Ekran yenilenmesini engellemeden hareket eder', 'Moves without blocking display refresh', 'Bewegt sich, ohne die Anzeigeaktualisierung zu blockieren', 'Se mueve sin bloquear la actualización de la pantalla', 'يتحرك دون تعطيل تحديث الشاشة'],
    ['Yön', 'Direction', 'Richtung', 'Dirección', 'الاتجاه'],
    ['Sola', 'Left', 'Links', 'Izquierda', 'يسار'],
    ['Sağa', 'Right', 'Rechts', 'Derecha', 'يمين'],
    ['Yukarı', 'Up', 'Oben', 'Arriba', 'أعلى'],
    ['Aşağı', 'Down', 'Unten', 'Abajo', 'أسفل'],
    ['Hız', 'Speed', 'Geschwindigkeit', 'Velocidad', 'السرعة'],
    ['Konum', 'Position', 'Position', 'Posición', 'الموضع'],
    ['Kutuyu Sil', 'Delete Box', 'Box löschen', 'Eliminar cuadro', 'حذف المربع'],

    ['STEP-RGB Ayarlar', 'STEP-RGB Settings', 'STEP-RGB Einstellungen', 'Ajustes de STEP-RGB', 'إعدادات STEP-RGB'],
    ['STEP-RGB panel kontrolü', 'STEP-RGB panel control', 'STEP-RGB Panelsteuerung', 'Control del panel STEP-RGB', 'التحكم في لوحة STEP-RGB'],
    ['Ayar grupları', 'Settings groups', 'Einstellungsgruppen', 'Grupos de ajustes', 'مجموعات الإعدادات'],
    ['Panel Ayarları', 'Panel Settings', 'Panel-Einstellungen', 'Ajustes del panel', 'إعدادات اللوحة'],
    ['Saat ve Tarih Ayarları', 'Time and Date Settings', 'Uhrzeit- und Datumseinstellungen', 'Ajustes de hora y fecha', 'إعدادات الوقت والتاريخ'],
    ['Alarm Ayarları', 'Alarm Settings', 'Alarm-Einstellungen', 'Ajustes de alarmas', 'إعدادات المنبهات'],
    ['Haberleşme Ayarları', 'Communication Settings', 'Kommunikationseinstellungen', 'Ajustes de comunicación', 'إعدادات الاتصال'],
    ['Wi-Fi Ayarları', 'Wi-Fi Settings', 'Wi-Fi-Einstellungen', 'Ajustes de Wi-Fi', 'إعدادات Wi-Fi'],
    ['Yazılım Güncelleme', 'Software Update', 'Softwareaktualisierung', 'Actualización de software', 'تحديث البرنامج'],
    ['Tarama ve yerleşim değişiklikleri kaydedildikten sonra cihaz otomatik yeniden başlar.', 'The device restarts automatically after scan and layout changes are saved.', 'Nach dem Speichern von Scan- und Layoutänderungen startet das Gerät automatisch neu.', 'El dispositivo se reinicia automáticamente después de guardar los cambios de escaneo y disposición.', 'يعاد تشغيل الجهاز تلقائياً بعد حفظ تغييرات المسح والتخطيط.'],
    ['Panel yenileme / tarama oranı', 'Panel refresh / scan ratio', 'Panel-Aktualisierung / Scanrate', 'Frecuencia de refresco / escaneo', 'معدل تحديث / مسح اللوحة'],
    ['Panel sayısı', 'Panel count', 'Panelanzahl', 'Número de paneles', 'عدد اللوحات'],
    ['Satır sayısı', 'Row count', 'Zeilenanzahl', 'Número de filas', 'عدد الصفوف'],
    ['Sütun sayısı', 'Column count', 'Spaltenanzahl', 'Número de columnas', 'عدد الأعمدة'],
    ['Parlaklık', 'Brightness', 'Helligkeit', 'Brillo', 'السطوع'],
    ['Panel bağlantı şekli', 'Panel connection layout', 'Panel-Verbindungsart', 'Disposición de conexión del panel', 'تخطيط توصيل اللوحة'],
    ['Sol üstten aşağı', 'Top left, downward', 'Oben links, abwärts', 'Desde arriba a la izquierda, hacia abajo', 'من أعلى اليسار إلى الأسفل'],
    ['Sağ üstten aşağı', 'Top right, downward', 'Oben rechts, abwärts', 'Desde arriba a la derecha, hacia abajo', 'من أعلى اليمين إلى الأسفل'],
    ['Sol alttan yukarı', 'Bottom left, upward', 'Unten links, aufwärts', 'Desde abajo a la izquierda, hacia arriba', 'من أسفل اليسار إلى الأعلى'],
    ['Sağ alttan yukarı', 'Bottom right, upward', 'Unten rechts, aufwärts', 'Desde abajo a la derecha, hacia arriba', 'من أسفل اليمين إلى الأعلى'],
    ['Sol üstten aşağı · ZZ', 'Top left, downward · ZZ', 'Oben links, abwärts · ZZ', 'Desde arriba a la izquierda, hacia abajo · ZZ', 'من أعلى اليسار إلى الأسفل · ZZ'],
    ['Sağ üstten aşağı · ZZ', 'Top right, downward · ZZ', 'Oben rechts, abwärts · ZZ', 'Desde arriba a la derecha, hacia abajo · ZZ', 'من أعلى اليمين إلى الأسفل · ZZ'],
    ['Sağ alttan yukarı · ZZ', 'Bottom right, upward · ZZ', 'Unten rechts, aufwärts · ZZ', 'Desde abajo a la derecha, hacia arriba · ZZ', 'من أسفل اليمين إلى الأعلى · ZZ'],
    ['Sol alttan yukarı · ZZ', 'Bottom left, upward · ZZ', 'Unten links, aufwärts · ZZ', 'Desde abajo a la izquierda, hacia arriba · ZZ', 'من أسفل اليسار إلى الأعلى · ZZ'],
    ['RGB panel testi', 'RGB panel test', 'RGB-Paneltest', 'Prueba del panel RGB', 'اختبار لوحة RGB'],
    ['Kırmızı', 'Red', 'Rot', 'Rojo', 'أحمر'],
    ['Yeşil', 'Green', 'Grün', 'Verde', 'أخضر'],
    ['Mavi', 'Blue', 'Blau', 'Azul', 'أزرق'],
    ['Panel Ayarlarını Kaydet', 'Save Panel Settings', 'Panel-Einstellungen speichern', 'Guardar ajustes del panel', 'حفظ إعدادات اللوحة'],
    ['Bilgiler DS3231 gerçek zaman saatine yazılır; elektrik kesildiğinde RTC piliyle korunur.', 'The values are written to the DS3231 real-time clock and retained by its RTC battery during a power outage.', 'Die Werte werden in die DS3231-Echtzeituhr geschrieben und bei Stromausfall durch die RTC-Batterie erhalten.', 'Los valores se escriben en el reloj de tiempo real DS3231 y se conservan con su batería RTC durante un corte de energía.', 'تُكتب القيم في ساعة DS3231 الحقيقية وتحفظها بطارية RTC عند انقطاع الكهرباء.'],
    ['Panel uyku ayarı', 'Panel sleep setting', 'Panel-Ruhezustand', 'Ajuste de reposo del panel', 'إعداد سكون اللوحة'],
    ['Belirlenen saatler arasında paneli karart', 'Dim the panel during the specified hours', 'Panel während der festgelegten Zeiten abdunkeln', 'Atenuar el panel durante las horas indicadas', 'تعتيم اللوحة خلال الساعات المحددة'],
    ['Başlangıç saati', 'Start time', 'Startzeit', 'Hora de inicio', 'وقت البدء'],
    ['Bitiş saati', 'End time', 'Endzeit', 'Hora de finalización', 'وقت الانتهاء'],
    ['Açılış günleri', 'Wake-up days', 'Einschalttage', 'Días de encendido', 'أيام التشغيل'],
    ['Haftanın her günü', 'Every day', 'Jeden Tag', 'Todos los días', 'كل يوم'],
    ['Sadece hafta içi', 'Weekdays only', 'Nur werktags', 'Solo días laborables', 'أيام الأسبوع فقط'],
    ['Panel her gün başlangıç saatinde karartılır. Bitiş saati seçili açılış günlerinin dışında kalırsa panel kapalı kalır ve ilk uygun günün bitiş saatinde açılır. Örneğin cuma 23:00’te kapanan panel, hafta içi seçiminde pazartesi 07:00’de açılır. Cihaz, Wi-Fi ve web arayüzü çalışmaya devam eder.', 'The panel dims at the start time each day. If the end time falls outside the selected wake-up days, it remains off until the end time on the first eligible day. For example, with weekdays selected, a panel dimmed Friday at 23:00 wakes Monday at 07:00. The device, Wi-Fi, and web interface keep running.', 'Das Panel wird täglich zur Startzeit abgedunkelt. Liegt die Endzeit außerhalb der gewählten Einschalttage, bleibt es bis zur Endzeit des nächsten gültigen Tages aus. Bei Auswahl der Werktage wird ein Freitag um 23:00 abgedunkeltes Panel beispielsweise Montag um 07:00 eingeschaltet. Gerät, Wi-Fi und Weboberfläche laufen weiter.', 'El panel se atenúa cada día a la hora de inicio. Si la hora final queda fuera de los días de encendido seleccionados, permanece apagado hasta la hora final del primer día válido. Por ejemplo, con días laborables, un panel apagado el viernes a las 23:00 se enciende el lunes a las 07:00. El dispositivo, Wi-Fi y la interfaz web siguen funcionando.', 'تُعتم اللوحة يومياً عند وقت البدء. إذا وقع وقت الانتهاء خارج أيام التشغيل المحددة فتبقى مطفأة حتى وقت الانتهاء في أول يوم مناسب. مثلاً عند اختيار أيام الأسبوع، تستيقظ اللوحة التي عُتمت الجمعة 23:00 يوم الاثنين 07:00. يستمر الجهاز وWi-Fi وواجهة الويب في العمل.'],
    ['Telefon/Bilgisayar Saatini Kullan', 'Use Phone/Computer Time', 'Telefon-/Computerzeit verwenden', 'Usar la hora del teléfono/ordenador', 'استخدام وقت الهاتف/الكمبيوتر'],
    ['Saat ve Tarihi Kaydet', 'Save Time and Date', 'Uhrzeit und Datum speichern', 'Guardar hora y fecha', 'حفظ الوقت والتاريخ'],
    ['Alarm tetiklendiğinde IO42 rölesi belirlenen süre boyunca açık kalır. En fazla 10 alarm oluşturabilirsiniz.', 'When an alarm triggers, the IO42 relay remains active for the specified duration. You can create up to 10 alarms.', 'Bei einem Alarm bleibt das IO42-Relais für die festgelegte Dauer aktiv. Es können bis zu 10 Alarme erstellt werden.', 'Cuando se activa una alarma, el relé IO42 permanece activo durante el tiempo indicado. Puede crear hasta 10 alarmas.', 'عند تشغيل المنبه يبقى مرحل IO42 فعالاً للمدة المحددة. يمكنك إنشاء ما يصل إلى 10 منبهات.'],
    ['Süre: 1–3600 saniye', 'Duration: 1–3600 seconds', 'Dauer: 1–3600 Sekunden', 'Duración: 1–3600 segundos', 'المدة: 1–3600 ثانية'],
    ['Henüz alarm oluşturulmadı.', 'No alarms have been created yet.', 'Es wurden noch keine Alarme erstellt.', 'Todavía no se han creado alarmas.', 'لم يتم إنشاء أي منبه بعد.'],
    ['Yeni Alarm Ekle', 'Add New Alarm', 'Neuen Alarm hinzufügen', 'Añadir nueva alarma', 'إضافة منبه جديد'],
    ['Alarm Ayarlarını Kaydet', 'Save Alarm Settings', 'Alarm-Einstellungen speichern', 'Guardar ajustes de alarmas', 'حفظ إعدادات المنبهات'],
    ['RS485 Modbus RTU hattı RX IO39, TX IO40 ve DIR IO41 pinlerini kullanır. Değişiklikler kaydedildikten sonra cihaz otomatik yeniden başlar.', 'The RS485 Modbus RTU bus uses RX IO39, TX IO40, and DIR IO41. The device restarts automatically after changes are saved.', 'Der RS485-Modbus-RTU-Bus verwendet RX IO39, TX IO40 und DIR IO41. Nach dem Speichern der Änderungen startet das Gerät automatisch neu.', 'El bus RS485 Modbus RTU usa RX IO39, TX IO40 y DIR IO41. El dispositivo se reinicia automáticamente después de guardar los cambios.', 'يستخدم خط RS485 Modbus RTU الأطراف RX IO39 وTX IO40 وDIR IO41. يعاد تشغيل الجهاز تلقائياً بعد حفظ التغييرات.'],
    ['Data size', 'Data bits', 'Datenbits', 'Bits de datos', 'بتات البيانات'],
    ['Parity', 'Parity', 'Parität', 'Paridad', 'التكافؤ'],
    ['Yok (N)', 'None (N)', 'Keine (N)', 'Ninguna (N)', 'بدون (N)'],
    ['Even (E)', 'Even (E)', 'Gerade (E)', 'Par (E)', 'زوجي (E)'],
    ['Odd (O)', 'Odd (O)', 'Ungerade (O)', 'Impar (O)', 'فردي (O)'],
    ['Handshake / yön kontrolü', 'Handshake / direction control', 'Handshake / Richtungssteuerung', 'Handshake / control de dirección', 'المصافحة / التحكم بالاتجاه'],
    ['RS485 DIR kontrolü (IO41)', 'RS485 DIR control (IO41)', 'RS485-DIR-Steuerung (IO41)', 'Control DIR RS485 (IO41)', 'تحكم DIR لـ RS485 ‏(IO41)'],
    ['Yok / otomatik yön donanımı', 'None / automatic direction hardware', 'Keine / automatische Richtungsumschaltung', 'Ninguno / hardware de dirección automática', 'بدون / عتاد اتجاه تلقائي'],
    ['ST3485 bağlantısında', 'With an ST3485 connection', 'Bei einer ST3485-Verbindung', 'Con una conexión ST3485', 'مع توصيل ST3485'],
    ['RS485 DIR kontrolü', 'RS485 DIR control', 'RS485-DIR-Steuerung', 'control DIR RS485', 'تحكم DIR لـ RS485'],
    ['kullanılmalıdır. “Yok” seçeneği yalnızca kendi yönünü otomatik değiştiren RS485 dönüştürücüler içindir. Stop bit sabit olarak 1’dir.', 'must be used. “None” is only for RS485 converters that switch direction automatically. The stop bit is fixed at 1.', 'muss verwendet werden. „Keine“ ist nur für RS485-Wandler mit automatischer Richtungsumschaltung vorgesehen. Das Stoppbit ist fest auf 1 eingestellt.', 'debe utilizarse. “Ninguno” es solo para convertidores RS485 que cambian de dirección automáticamente. El bit de parada está fijado en 1.', 'يجب استخدامه. خيار «بدون» مخصص فقط لمحولات RS485 التي تغير اتجاهها تلقائياً. بت التوقف ثابت على 1.'],
    ['Haberleşme Ayarlarını Kaydet', 'Save Communication Settings', 'Kommunikationseinstellungen speichern', 'Guardar ajustes de comunicación', 'حفظ إعدادات الاتصال'],
    ['STA bilgileri girilmişse cihaz bağlantıyı her 60 saniyede bir, en fazla 10 saniye boyunca dener.', 'When STA credentials are configured, the device attempts to connect every 60 seconds for up to 10 seconds.', 'Wenn STA-Zugangsdaten gespeichert sind, versucht das Gerät alle 60 Sekunden bis zu 10 Sekunden lang eine Verbindung herzustellen.', 'Cuando se configuran datos STA, el dispositivo intenta conectarse cada 60 segundos durante un máximo de 10 segundos.', 'عند ضبط بيانات STA يحاول الجهاز الاتصال كل 60 ثانية لمدة تصل إلى 10 ثوانٍ.'],
    ['STA durumu alınıyor', 'Loading STA status', 'STA-Status wird geladen', 'Cargando estado STA', 'جارٍ تحميل حالة STA'],
    ['Lütfen bekleyin…', 'Please wait…', 'Bitte warten…', 'Espere…', 'يرجى الانتظار…'],
    ['Mevcut cihaz ağı (AP)', 'Current device network (AP)', 'Aktuelles Gerätenetzwerk (AP)', 'Red actual del dispositivo (AP)', 'شبكة الجهاز الحالية (AP)'],
    ['Bağlanılacak ağ (STA)', 'Network to join (STA)', 'Zu verbindendes Netzwerk (STA)', 'Red a la que conectarse (STA)', 'الشبكة المطلوب الاتصال بها (STA)'],
    ['Yalnızca 2.4 GHz:', '2.4 GHz only:', 'Nur 2,4 GHz:', 'Solo 2,4 GHz:', '2.4 غيغاهرتز فقط:'],
    ['ESP32-S3, 5 GHz Wi-Fi ağlarına bağlanamaz. STA için 2.4 GHz yayın yapan bir SSID seçin.', 'ESP32-S3 cannot connect to 5 GHz Wi-Fi networks. Select an SSID broadcasting on 2.4 GHz for STA.', 'Der ESP32-S3 kann keine Verbindung zu 5-GHz-Wi-Fi-Netzen herstellen. Für STA eine SSID im 2,4-GHz-Band wählen.', 'ESP32-S3 no puede conectarse a redes Wi-Fi de 5 GHz. Seleccione un SSID que emita en 2,4 GHz para STA.', 'لا يستطيع ESP32-S3 الاتصال بشبكات Wi-Fi بتردد 5 غيغاهرتز. اختر SSID يبث على 2.4 غيغاهرتز لوضع STA.'],
    ['Parola', 'Password', 'Passwort', 'Contraseña', 'كلمة المرور'],
    ['Boş bırakılırsa devre dışı', 'Leave blank to disable', 'Zum Deaktivieren leer lassen', 'Dejar vacío para desactivar', 'اتركه فارغاً للتعطيل'],
    ['Açık ağ için boş bırakılabilir', 'May be left blank for an open network', 'Bei einem offenen Netzwerk leer lassen', 'Puede dejarse vacío para una red abierta', 'يمكن تركه فارغاً للشبكة المفتوحة'],
    ['Wi-Fi Ayarlarını Kaydet', 'Save Wi-Fi Settings', 'Wi-Fi-Einstellungen speichern', 'Guardar ajustes de Wi-Fi', 'حفظ إعدادات Wi-Fi'],
    ['Bilgisayarınızdan veya telefonunuzdan OTA firmware dosyasını seçin. Yükleme tamamlandığında cihaz otomatik olarak yeniden başlar.', 'Select the OTA firmware file from your computer or phone. The device restarts automatically when the upload completes.', 'OTA-Firmwaredatei auf Computer oder Telefon auswählen. Nach Abschluss des Uploads startet das Gerät automatisch neu.', 'Seleccione el archivo de firmware OTA desde el ordenador o el teléfono. El dispositivo se reinicia automáticamente al finalizar la carga.', 'اختر ملف برنامج OTA من الكمبيوتر أو الهاتف. يعاد تشغيل الجهاز تلقائياً عند اكتمال الرفع.'],
    ['Firmware dosyası', 'Firmware file', 'Firmwaredatei', 'Archivo de firmware', 'ملف البرنامج'],
    ['Gerekli dosya adı:', 'Required file name:', 'Erforderlicher Dateiname:', 'Nombre de archivo requerido:', 'اسم الملف المطلوب:'],
    ['Firmware yükleme ilerlemesi', 'Firmware upload progress', 'Fortschritt des Firmware-Uploads', 'Progreso de carga del firmware', 'تقدم رفع البرنامج'],
    ['Yükleme bekleniyor.', 'Waiting for upload.', 'Upload wird erwartet.', 'Esperando la carga.', 'في انتظار الرفع.'],
    ['Güncellemeyi Başlat', 'Start Update', 'Aktualisierung starten', 'Iniciar actualización', 'بدء التحديث'],

    ['Döngü: {types}', 'Cycle: {types}', 'Zyklus: {types}', 'Ciclo: {types}', 'الدورة: {types}'],
    ['Panel {number}', 'Panel {number}', 'Panel {number}', 'Panel {number}', 'اللوحة {number}'],
    ['seçili', 'selected', 'ausgewählt', 'seleccionado', 'محددة'],
    ['öğe', 'items', 'Elemente', 'elementos', 'عنصر'],
    ['{columns} × {rows} panel', '{columns} × {rows} panels', '{columns} × {rows} Panels', '{columns} × {rows} paneles', '{columns} × {rows} لوحات'],
    ['{width} × {height} piksel', '{width} × {height} pixels', '{width} × {height} Pixel', '{width} × {height} píxeles', '{width} × {height} بكسل'],
    ['Yeni {box} eklendi.', 'New {box} added.', 'Neue {box} hinzugefügt.', 'Se añadió un nuevo {box}.', 'تمت إضافة {box} جديد.'],
    ['{box} silindi. Değişikliği uygulayabilirsiniz.', '{box} deleted. You can apply the change.', '{box} gelöscht. Die Änderung kann angewendet werden.', '{box} eliminado. Puede aplicar el cambio.', 'تم حذف {box}. يمكنك تطبيق التغيير.'],
    ['{box} Ayarları', '{box} Settings', '{box}-Einstellungen', 'Ajustes de {box}', 'إعدادات {box}'],
    ['{box} Sil', 'Delete {box}', '{box} löschen', 'Eliminar {box}', 'حذف {box}'],
    ['{speed} px/sn', '{speed} px/s', '{speed} px/s', '{speed} px/s', '{speed} بكسل/ث'],
    ['Alarm {number}', 'Alarm {number}', 'Alarm {number}', 'Alarma {number}', 'المنبه {number}'],
    ['{count} / {maximum} alarm', '{count} / {maximum} alarms', '{count} / {maximum} Alarme', '{count} / {maximum} alarmas', '{count} / {maximum} منبهات'],
    ['En fazla {maximum} alarm oluşturabilirsiniz.', 'You can create up to {maximum} alarms.', 'Es können höchstens {maximum} Alarme erstellt werden.', 'Puede crear hasta {maximum} alarmas.', 'يمكنك إنشاء ما يصل إلى {maximum} منبهات.'],
    ['Sonraki denemeye {seconds} saniye kaldı.', '{seconds} seconds until the next attempt.', 'Noch {seconds} Sekunden bis zum nächsten Versuch.', 'Quedan {seconds} segundos para el siguiente intento.', 'تبقى {seconds} ثانية حتى المحاولة التالية.'],
    ['“{ssid}” ağına bağlanılıyor…', 'Connecting to “{ssid}”…', 'Verbindung mit „{ssid}“ wird hergestellt…', 'Conectando a “{ssid}”…', 'جارٍ الاتصال بشبكة «{ssid}»…'],
    ['“{ssid}” ağına bağlandı.', 'Connected to “{ssid}”.', 'Mit „{ssid}“ verbunden.', 'Conectado a “{ssid}”.', 'تم الاتصال بشبكة «{ssid}».'],
    ['Sinyal {quality}% ({rssi} dBm)', 'Signal {quality}% ({rssi} dBm)', 'Signal {quality}% ({rssi} dBm)', 'Señal {quality}% ({rssi} dBm)', 'الإشارة {quality}% ({rssi} dBm)'],
    ['bayt', 'bytes', 'Byte', 'bytes', 'بايت'],

    ['Alarm', 'Alarm', 'Alarm', 'Alarma', 'منبه'],
    ['Sil', 'Delete', 'Löschen', 'Eliminar', 'حذف'],
    ['Alarm saati', 'Alarm time', 'Alarmzeit', 'Hora de la alarma', 'وقت المنبه'],
    ['Tetiklenme süresi (saniye)', 'Trigger duration (seconds)', 'Auslösedauer (Sekunden)', 'Duración de activación (segundos)', 'مدة التشغيل (بالثواني)'],
    ['Tekrarlama günleri', 'Repeat days', 'Wiederholungstage', 'Días de repetición', 'أيام التكرار'],
    ['İstediğim günler', 'Custom days', 'Benutzerdefinierte Tage', 'Días personalizados', 'أيام مخصصة'],
    ['Alarmın çalışacağı günleri seçin.', 'Select the days on which the alarm will run.', 'Tage auswählen, an denen der Alarm aktiv ist.', 'Seleccione los días en que funcionará la alarma.', 'اختر الأيام التي يعمل فيها المنبه.'],
    ['Pazartesi', 'Monday', 'Montag', 'Lunes', 'الاثنين'],
    ['Salı', 'Tuesday', 'Dienstag', 'Martes', 'الثلاثاء'],
    ['Çarşamba', 'Wednesday', 'Mittwoch', 'Miércoles', 'الأربعاء'],
    ['Perşembe', 'Thursday', 'Donnerstag', 'Jueves', 'الخميس'],
    ['Cuma', 'Friday', 'Freitag', 'Viernes', 'الجمعة'],
    ['Cumartesi', 'Saturday', 'Samstag', 'Sábado', 'السبت'],
    ['Pazar', 'Sunday', 'Sonntag', 'Domingo', 'الأحد'],

    ['Yazı tipi önizleme verisi geçersiz.', 'Invalid font preview data.', 'Ungültige Schriftvorschau-Daten.', 'Datos de vista previa de fuente no válidos.', 'بيانات معاينة الخط غير صالحة.'],
    ['Yazı tipi önizleme verisi eksik.', 'Incomplete font preview data.', 'Unvollständige Schriftvorschau-Daten.', 'Faltan datos de vista previa de la fuente.', 'بيانات معاينة الخط غير مكتملة.'],
    ['Yazı tipi önizleme verisi alınamadı.', 'Could not load font preview data.', 'Schriftvorschau-Daten konnten nicht geladen werden.', 'No se pudieron cargar los datos de vista previa de la fuente.', 'تعذر تحميل بيانات معاينة الخط.'],
    ['Geçersiz cihaz yanıtı.', 'Invalid device response.', 'Ungültige Geräteantwort.', 'Respuesta del dispositivo no válida.', 'استجابة الجهاز غير صالحة.'],
    ['İşlem başarısız.', 'The operation failed.', 'Vorgang fehlgeschlagen.', 'La operación ha fallado.', 'فشلت العملية.'],
    ['İşlem tamamlandı.', 'Operation completed.', 'Vorgang abgeschlossen.', 'Operación completada.', 'اكتملت العملية.'],
    ['Hedef panelde toplam en fazla 2 TextBox/ValueBox olabilir.', 'The target panel can contain at most 2 TextBox/ValueBox items.', 'Das Zielpanel darf höchstens 2 TextBox/ValueBox-Elemente enthalten.', 'El panel de destino puede contener como máximo 2 elementos TextBox/ValueBox.', 'يمكن أن تحتوي اللوحة المستهدفة على عنصري TextBox/ValueBox كحد أقصى.'],
    ['Konum değişti. Kaydetmeyi unutmayın.', 'Position changed. Remember to save.', 'Position geändert. Speichern nicht vergessen.', 'Posición cambiada. No olvide guardar.', 'تم تغيير الموضع. لا تنس الحفظ.'],
    ['Veri türü değişti.', 'Data type changed.', 'Datentyp geändert.', 'Tipo de datos cambiado.', 'تم تغيير نوع البيانات.'],
    ['İçerik döngüsü açıldı.', 'Content cycle enabled.', 'Inhaltszyklus aktiviert.', 'Ciclo de contenido activado.', 'تم تفعيل دورة المحتوى.'],
    ['İçerik döngüsü kapatıldı.', 'Content cycle disabled.', 'Inhaltszyklus deaktiviert.', 'Ciclo de contenido desactivado.', 'تم تعطيل دورة المحتوى.'],
    ['Döngü için en az bir bilgi seçilmelidir.', 'Select at least one item for the cycle.', 'Mindestens eine Information für den Zyklus auswählen.', 'Seleccione al menos un dato para el ciclo.', 'اختر معلومة واحدة على الأقل للدورة.'],
    ['Sıcaklık gösterim tipi değişti.', 'Temperature display type changed.', 'Temperaturanzeigetyp geändert.', 'Tipo de visualización de temperatura cambiado.', 'تم تغيير نوع عرض درجة الحرارة.'],
    ['ValueBox veri kaynağı değişti.', 'ValueBox data source changed.', 'ValueBox-Datenquelle geändert.', 'Fuente de datos de ValueBox cambiada.', 'تم تغيير مصدر بيانات ValueBox.'],
    ['Yazı tipi önizlemesi hazırlanıyor…', 'Preparing font preview…', 'Schriftvorschau wird vorbereitet…', 'Preparando vista previa de la fuente…', 'جارٍ إعداد معاينة الخط…'],
    ['Yazı tipi değişti.', 'Font changed.', 'Schriftart geändert.', 'Fuente cambiada.', 'تم تغيير الخط.'],
    ['Kutu yönü değişti.', 'Box orientation changed.', 'Box-Ausrichtung geändert.', 'Orientación del cuadro cambiada.', 'تم تغيير اتجاه المربع.'],
    ['Panel içeriği hazır.', 'Panel content is ready.', 'Panelinhalt ist bereit.', 'El contenido del panel está listo.', 'محتوى اللوحة جاهز.'],
    ['Yazı tipi seçimi geçersiz.', 'Invalid font selection.', 'Ungültige Schriftauswahl.', 'Selección de fuente no válida.', 'اختيار الخط غير صالح.'],
    ['Yazı içeriği boş bırakılamaz.', 'Text content cannot be empty.', 'Textinhalt darf nicht leer sein.', 'El contenido de texto no puede estar vacío.', 'لا يمكن ترك محتوى النص فارغاً.'],
    ['Metin en fazla 47 bayt olabilir.', 'Text can contain at most 47 bytes.', 'Text darf höchstens 47 Byte lang sein.', 'El texto puede tener como máximo 47 bytes.', 'يمكن أن يكون النص 47 بايت كحد أقصى.'],
    ['Değişim sıklığı 1-3600 saniye olmalıdır.', 'The change interval must be 1–3600 seconds.', 'Das Wechselintervall muss 1–3600 Sekunden betragen.', 'El intervalo de cambio debe ser de 1–3600 segundos.', 'يجب أن يكون فاصل التغيير من 1 إلى 3600 ثانية.'],
    ['ValueBox başlangıç değeri geçersiz.', 'Invalid ValueBox initial value.', 'Ungültiger ValueBox-Startwert.', 'Valor inicial de ValueBox no válido.', 'القيمة الابتدائية لـ ValueBox غير صالحة.'],
    ['Dijital sayaç başlangıç değeri 32-bit bir integer olmalıdır.', 'The digital counter initial value must be a 32-bit integer.', 'Der Startwert des Digitalzählers muss eine 32-Bit-Ganzzahl sein.', 'El valor inicial del contador digital debe ser un entero de 32 bits.', 'يجب أن تكون القيمة الابتدائية للعداد الرقمي عدداً صحيحاً 32 بت.'],
    ['Modbus integer başlangıç değeri -32768 ile 32767 arasında olmalıdır.', 'The Modbus integer initial value must be between -32768 and 32767.', 'Der Modbus-Integer-Startwert muss zwischen -32768 und 32767 liegen.', 'El valor inicial entero Modbus debe estar entre -32768 y 32767.', 'يجب أن تكون القيمة الصحيحة الابتدائية لـ Modbus بين ‎-32768 و32767.'],
    ['ValueBox register adresi geçersiz.', 'Invalid ValueBox register address.', 'Ungültige ValueBox-Registeradresse.', 'Dirección de registro de ValueBox no válida.', 'عنوان سجل ValueBox غير صالح.'],
    ['Modbus ValueBox register adresleri çakışamaz.', 'Modbus ValueBox register addresses cannot overlap.', 'Modbus-ValueBox-Registeradressen dürfen sich nicht überschneiden.', 'Las direcciones de registro Modbus de ValueBox no pueden solaparse.', 'لا يمكن أن تتداخل عناوين سجلات Modbus لـ ValueBox.'],
    ['Panele uygulanıyor…', 'Applying to panel…', 'Wird auf das Panel angewendet…', 'Aplicando al panel…', 'جارٍ التطبيق على اللوحة…'],
    ['İçerik panele uygulandı ve flash belleğe kaydedildi.', 'Content was applied to the panel and saved to flash.', 'Inhalt wurde auf das Panel angewendet und im Flash gespeichert.', 'El contenido se aplicó al panel y se guardó en la memoria flash.', 'تم تطبيق المحتوى على اللوحة وحفظه في ذاكرة الفلاش.'],
    ['Alarm için en az bir gün seçilmelidir.', 'Select at least one day for the alarm.', 'Mindestens einen Tag für den Alarm auswählen.', 'Seleccione al menos un día para la alarma.', 'اختر يوماً واحداً على الأقل للمنبه.'],
    ['Özel alarm için en az bir gün seçilmelidir.', 'Select at least one day for a custom alarm.', 'Mindestens einen Tag für einen benutzerdefinierten Alarm auswählen.', 'Seleccione al menos un día para una alarma personalizada.', 'اختر يوماً واحداً على الأقل للمنبه المخصص.'],
    ['STA devre dışı', 'STA disabled', 'STA deaktiviert', 'STA desactivado', 'STA معطل'],
    ['Bağlantı bekleniyor', 'Waiting to connect', 'Warten auf Verbindung', 'Esperando conexión', 'في انتظار الاتصال'],
    ['STA bağlantısı bekletiliyor', 'STA connection paused', 'STA-Verbindung pausiert', 'Conexión STA en pausa', 'اتصال STA متوقف مؤقتاً'],
    ['Ağa bağlanılıyor', 'Connecting to network', 'Verbindung zum Netzwerk', 'Conectando a la red', 'جارٍ الاتصال بالشبكة'],
    ['STA bağlantısı başarılı', 'STA connected', 'STA verbunden', 'STA conectado', 'تم اتصال STA'],
    ['STA bağlantısı başarısız', 'STA connection failed', 'STA-Verbindung fehlgeschlagen', 'Conexión STA fallida', 'فشل اتصال STA'],
    ['Kaydedilen Wi-Fi ağı bulunamadı.', 'The saved Wi-Fi network was not found.', 'Das gespeicherte Wi-Fi-Netzwerk wurde nicht gefunden.', 'No se encontró la red Wi-Fi guardada.', 'لم يتم العثور على شبكة Wi-Fi المحفوظة.'],
    ['Erişim noktası bağlantıyı kabul etmedi; SSID ve parolayı kontrol edin.', 'The access point rejected the connection; check the SSID and password.', 'Der Access Point hat die Verbindung abgelehnt; SSID und Passwort prüfen.', 'El punto de acceso rechazó la conexión; compruebe el SSID y la contraseña.', 'رفضت نقطة الوصول الاتصال؛ تحقق من SSID وكلمة المرور.'],
    ['Kablosuz bağlantı kesildi.', 'The wireless connection was lost.', 'Die Funkverbindung wurde getrennt.', 'Se perdió la conexión inalámbrica.', 'انقطع الاتصال اللاسلكي.'],
    ['10 saniye içinde bağlantı kurulamadı.', 'Could not connect within 10 seconds.', 'Verbindung konnte nicht innerhalb von 10 Sekunden hergestellt werden.', 'No se pudo conectar en 10 segundos.', 'تعذر الاتصال خلال 10 ثوانٍ.'],
    ['STA bağlantı durumu cihazdan alınamadı.', 'Could not read STA status from the device.', 'STA-Status konnte nicht vom Gerät gelesen werden.', 'No se pudo obtener el estado STA del dispositivo.', 'تعذر قراءة حالة STA من الجهاز.'],
    ['Yeni bağlantı denemesi başlatılıyor…', 'Starting a new connection attempt…', 'Neuer Verbindungsversuch wird gestartet…', 'Iniciando un nuevo intento de conexión…', 'جارٍ بدء محاولة اتصال جديدة…'],
    ['Bir sonraki otomatik bağlantı denemesi bekleniyor.', 'Waiting for the next automatic connection attempt.', 'Warten auf den nächsten automatischen Verbindungsversuch.', 'Esperando el siguiente intento de conexión automático.', 'في انتظار محاولة الاتصال التلقائية التالية.'],
    ['Bağlanılacak bir STA ağı kaydedilmemiş.', 'No STA network has been saved.', 'Es wurde kein STA-Netzwerk gespeichert.', 'No se ha guardado ninguna red STA.', 'لم يتم حفظ شبكة STA.'],
    ['Yerel AP arayüzüne bir cihaz bağlı olduğu için yeni STA denemeleri bekletiliyor.', 'New STA attempts are paused while a device is connected to the local AP.', 'Neue STA-Versuche sind pausiert, solange ein Gerät mit dem lokalen AP verbunden ist.', 'Los nuevos intentos STA están en pausa mientras haya un dispositivo conectado al AP local.', 'محاولات STA الجديدة متوقفة مؤقتاً أثناء اتصال جهاز بنقطة الوصول المحلية.'],
    ['Bağlantı kurulamadı.', 'Could not connect.', 'Verbindung konnte nicht hergestellt werden.', 'No se pudo conectar.', 'تعذر الاتصال.'],
    ['Geçersiz dosya.', 'Invalid file.', 'Ungültige Datei.', 'Archivo no válido.', 'ملف غير صالح.'],
    ['Dosya adı tam olarak firmware.bin olmalıdır.', 'The file name must be exactly firmware.bin.', 'Der Dateiname muss exakt firmware.bin lauten.', 'El nombre del archivo debe ser exactamente firmware.bin.', 'يجب أن يكون اسم الملف firmware.bin تماماً.'],
    ['Firmware yüklenmeye hazırlanıyor…', 'Preparing firmware upload…', 'Firmware-Upload wird vorbereitet…', 'Preparando la carga del firmware…', 'جارٍ إعداد رفع البرنامج…'],
    ['Firmware doğrulanıyor…', 'Verifying firmware…', 'Firmware wird geprüft…', 'Verificando el firmware…', 'جارٍ التحقق من البرنامج…'],
    ['Firmware cihaza yükleniyor…', 'Uploading firmware to the device…', 'Firmware wird auf das Gerät geladen…', 'Cargando el firmware en el dispositivo…', 'جارٍ رفع البرنامج إلى الجهاز…'],
    ['Güncelleme tamamlandı. Cihaz yeniden başlatılıyor.', 'Update complete. The device is restarting.', 'Aktualisierung abgeschlossen. Das Gerät wird neu gestartet.', 'Actualización completada. El dispositivo se está reiniciando.', 'اكتمل التحديث. جارٍ إعادة تشغيل الجهاز.'],
    ['Güncelleme tamamlandı.', 'Update complete.', 'Aktualisierung abgeschlossen.', 'Actualización completada.', 'اكتمل التحديث.'],
    ['Firmware güncellenemedi.', 'Firmware could not be updated.', 'Firmware konnte nicht aktualisiert werden.', 'No se pudo actualizar el firmware.', 'تعذر تحديث البرنامج.'],
    ['Cihaza bağlantı kesildi. Firmware güncellenemedi.', 'Connection to the device was lost. Firmware could not be updated.', 'Verbindung zum Gerät wurde getrennt. Firmware konnte nicht aktualisiert werden.', 'Se perdió la conexión con el dispositivo. No se pudo actualizar el firmware.', 'انقطع الاتصال بالجهاز. تعذر تحديث البرنامج.'],
    ['Firmware yüklemesi iptal edildi.', 'Firmware upload canceled.', 'Firmware-Upload abgebrochen.', 'Carga de firmware cancelada.', 'تم إلغاء رفع البرنامج.'],

    ['Ekran tasarimi panele uygulandi ve kaydedildi.', 'The display design was applied and saved.', 'Das Anzeigedesign wurde angewendet und gespeichert.', 'El diseño de pantalla se aplicó y se guardó.', 'تم تطبيق تصميم الشاشة وحفظه.'],
    ['Panel ayarlari kaydedildi. Cihaz yeniden baslatilacak.', 'Panel settings saved. The device will restart.', 'Panel-Einstellungen gespeichert. Das Gerät wird neu gestartet.', 'Ajustes del panel guardados. El dispositivo se reiniciará.', 'تم حفظ إعدادات اللوحة. سيعاد تشغيل الجهاز.'],
    ['Saat, tarih ve panel uyku ayarlari kaydedildi.', 'Time, date, and panel sleep settings saved.', 'Uhrzeit, Datum und Panel-Ruhezustand gespeichert.', 'Ajustes de hora, fecha y reposo del panel guardados.', 'تم حفظ إعدادات الوقت والتاريخ وسكون اللوحة.'],
    ['Alarm ayarlari kaydedildi.', 'Alarm settings saved.', 'Alarm-Einstellungen gespeichert.', 'Ajustes de alarmas guardados.', 'تم حفظ إعدادات المنبهات.'],
    ['Wi-Fi ayarlari kaydedildi. Cihaz yeniden baslatilacak.', 'Wi-Fi settings saved. The device will restart.', 'Wi-Fi-Einstellungen gespeichert. Das Gerät wird neu gestartet.', 'Ajustes de Wi-Fi guardados. El dispositivo se reiniciará.', 'تم حفظ إعدادات Wi-Fi. سيعاد تشغيل الجهاز.'],
    ['Haberlesme ayarlari kaydedildi. Cihaz yeniden baslatilacak.', 'Communication settings saved. The device will restart.', 'Kommunikationseinstellungen gespeichert. Das Gerät wird neu gestartet.', 'Ajustes de comunicación guardados. El dispositivo se reiniciará.', 'تم حفظ إعدادات الاتصال. سيعاد تشغيل الجهاز.'],
    ['Renk testi baslatildi.', 'Color test started.', 'Farbtest gestartet.', 'Prueba de color iniciada.', 'بدأ اختبار اللون.']
  ];

  const dictionaries = {en: {}, de: {}, es: {}, ar: {}};
  rows.forEach(row => {
    dictionaries.en[row[0]] = row[1];
    dictionaries.de[row[0]] = row[2];
    dictionaries.es[row[0]] = row[3];
    dictionaries.ar[row[0]] = row[4];
  });

  const textSources = new WeakMap();
  const attributeSources = new WeakMap();
  const listeners = new Set();
  let initialized = false;
  let language = initialLanguage();
  let titleSource = '';

  function initialLanguage() {
    try {
      const stored = localStorage.getItem(languageStorageKey);
      if (supportedLanguages.includes(stored)) return stored;
    } catch (_) {}
    const candidates = Array.isArray(navigator.languages)
      ? navigator.languages
      : [navigator.language];
    for (const candidate of candidates) {
      const code = String(candidate || '').toLowerCase().split('-')[0];
      if (supportedLanguages.includes(code)) return code;
    }
    return 'tr';
  }

  function translated(source) {
    if (language === 'tr') return source;
    return dictionaries[language][source] || source;
  }

  function t(source, values = {}) {
    let value = translated(String(source ?? ''));
    Object.keys(values).forEach(key => {
      value = value.split(`{${key}}`).join(String(values[key]));
    });
    return value;
  }

  function server(message, fallback = 'İşlem başarısız.') {
    const source = String(message || '');
    if (!source) return t(fallback);
    if (language === 'tr' || dictionaries[language][source]) return t(source);
    return t(fallback);
  }

  function localizeTextNode(node) {
    let source = textSources.get(node);
    if (!source) {
      const match = String(node.nodeValue || '').match(/^(\s*)([\s\S]*?)(\s*)$/);
      if (!match || !match[2]) return;
      source = {prefix: match[1], text: match[2], suffix: match[3]};
      textSources.set(node, source);
    }
    node.nodeValue = source.prefix + t(source.text) + source.suffix;
  }

  function localizeAttributes(element) {
    const names = ['aria-label', 'placeholder', 'title'];
    let sources = attributeSources.get(element);
    if (!sources) {
      sources = {};
      names.forEach(name => {
        if (element.hasAttribute(name)) sources[name] = element.getAttribute(name);
      });
      attributeSources.set(element, sources);
    }
    Object.keys(sources).forEach(name => element.setAttribute(name, t(sources[name])));
  }

  function localize(root = document) {
    const rootElement = root.nodeType === Node.ELEMENT_NODE ? root : document.documentElement;
    if (rootElement) {
      localizeAttributes(rootElement);
      rootElement.querySelectorAll('*').forEach(element => {
        if (element.tagName !== 'SCRIPT' && element.tagName !== 'STYLE') {
          localizeAttributes(element);
        }
      });
    }
    const walker = document.createTreeWalker(root, NodeFilter.SHOW_TEXT, {
      acceptNode(node) {
        const parent = node.parentElement;
        return parent && parent.tagName !== 'SCRIPT' && parent.tagName !== 'STYLE'
          ? NodeFilter.FILTER_ACCEPT
          : NodeFilter.FILTER_REJECT;
      }
    });
    let node;
    while ((node = walker.nextNode())) localizeTextNode(node);
  }

  function apply() {
    document.documentElement.lang = language;
    document.documentElement.dir = language === 'ar' ? 'rtl' : 'ltr';
    if (titleSource) document.title = t(titleSource);
    const select = document.getElementById('languageSelect');
    if (select) select.value = language;
    localize(document);
  }

  function setLanguage(nextLanguage) {
    if (!supportedLanguages.includes(nextLanguage)) return;
    language = nextLanguage;
    try { localStorage.setItem(languageStorageKey, language); } catch (_) {}
    apply();
    listeners.forEach(listener => listener(language));
  }

  function init() {
    if (initialized) return;
    initialized = true;
    titleSource = document.title;
    const select = document.getElementById('languageSelect');
    if (select) {
      select.value = language;
      select.addEventListener('change', event => setLanguage(event.target.value));
    }
    apply();
  }

  window.StepRgbI18n = Object.freeze({
    init,
    localize,
    onChange(listener) { listeners.add(listener); return () => listeners.delete(listener); },
    server,
    setLanguage,
    t,
    get language() { return language; }
  });
})();
