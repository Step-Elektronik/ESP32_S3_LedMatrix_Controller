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
    ['Nem', 'Humidity', 'Feuchte', 'Humedad', 'الرطوبة'],
    ['İçerik döngüsü', 'Content cycle', 'Inhaltszyklus', 'Ciclo de contenido', 'دورة المحتوى'],
    ["Birden fazla bilgiyi aynı TextBox'ta sırayla gösterir", 'Shows multiple pieces of information in the same TextBox in sequence', 'Zeigt mehrere Informationen nacheinander in derselben TextBox an', 'Muestra varios datos secuencialmente en el mismo TextBox', 'يعرض عدة معلومات بالتتابع في TextBox نفسه'],
    ['İşaretlenen bilgiler Yazı → Saat → Tarih → Sıcaklık → Nem sırasıyla gösterilir. Kayar yazı açıksa içerik tam turunu tamamladıktan sonra değişir.', 'Selected items are shown in the order Text → Time → Date → Temperature → Humidity. With scrolling text on, the content changes after a complete pass.', 'Ausgewählte Inhalte werden in der Reihenfolge Text → Uhrzeit → Datum → Temperatur → Feuchte angezeigt. Bei Lauftext wechselt der Inhalt nach einem vollständigen Durchlauf.', 'Los elementos seleccionados se muestran en el orden Texto → Hora → Fecha → Temperatura → Humedad. Con el texto desplazable activo, el contenido cambia tras una pasada completa.', 'تُعرض العناصر المحددة بالترتيب نص ← وقت ← تاريخ ← درجة الحرارة ← الرطوبة. عند تفعيل النص المتحرك يتغير المحتوى بعد اكتمال دورة كاملة.'],
    ['Değişim sıklığı (saniye)', 'Change interval (seconds)', 'Wechselintervall (Sekunden)', 'Intervalo de cambio (segundos)', 'فاصل التغيير (بالثواني)'],
    ['Yazdırılacak metin', 'Text to display', 'Anzuzeigender Text', 'Texto que se mostrará', 'النص المراد عرضه'],
    ['Metninizi yazın', 'Enter your text', 'Text eingeben', 'Escriba el texto', 'اكتب النص'],
    ['Sıcaklık ve nem gösterimi', 'Temperature and humidity format', 'Format für Temperatur und Feuchte', 'Formato de temperatura y humedad', 'تنسيق درجة الحرارة والرطوبة'],
    ['Ondalıklı — bir basamak (23.5°C, 45.3%)', 'Decimal — one place (23.5°C, 45.3%)', 'Dezimal — eine Stelle (23.5°C, 45.3%)', 'Decimal — una cifra (23.5°C, 45.3%)', 'عشري — منزلة واحدة (23.5°C، 45.3%)'],
    ['Tam sayı — yuvarlanmış (24°C, 45%)', 'Whole number — rounded (24°C, 45%)', 'Ganzzahl — gerundet (24°C, 45%)', 'Entero — redondeado (24°C, 45%)', 'عدد صحيح — مُقرَّب (24°C، 45%)'],
    ['Tam sayı seçiminde en yakın tam sayıya yuvarlanır; 0.5 ve üzeri bir üst değere çıkar.', 'Whole numbers round to the nearest integer; 0.5 and above rounds up.', 'Ganzzahlen werden auf die nächste ganze Zahl gerundet; ab 0,5 wird aufgerundet.', 'Los enteros se redondean al entero más cercano; 0,5 o más redondea hacia arriba.', 'يُقرَّب العدد الصحيح إلى أقرب عدد صحيح؛ وتُقرَّب القيمة 0.5 فأكثر إلى الأعلى.'],
    ['Seçili sensör ({sensor}) nem ölçmez. Sensör, Ayarlar → Sıcaklık Sensörü bölümünden değiştirilebilir.', 'The selected sensor ({sensor}) does not measure humidity. The sensor can be changed under Settings → Temperature Sensor.', 'Der gewählte Sensor ({sensor}) misst keine Feuchte. Der Sensor kann unter Einstellungen → Temperatursensor geändert werden.', 'El sensor seleccionado ({sensor}) no mide la humedad. Puede cambiarse en Ajustes → Sensor de temperatura.', 'المستشعر المحدد ({sensor}) لا يقيس الرطوبة. يمكن تغيير المستشعر من الإعدادات ← مستشعر درجة الحرارة.'],
    ['Seçili sensör ({sensor}) nem ölçmez; nem gösteren kutuları değiştirin.', 'The selected sensor ({sensor}) does not measure humidity; change the boxes that show humidity.', 'Der gewählte Sensor ({sensor}) misst keine Feuchte; ändern Sie die Boxen, die Feuchte anzeigen.', 'El sensor seleccionado ({sensor}) no mide la humedad; cambie los cuadros que muestran humedad.', 'المستشعر المحدد ({sensor}) لا يقيس الرطوبة؛ غيّر المربعات التي تعرض الرطوبة.'],
    ['Başlangıç değeri', 'Initial value', 'Startwert', 'Valor inicial', 'القيمة الابتدائية'],
    ['Cihaz her açıldığında ValueBox bu değerden başlar.', 'The ValueBox starts with this value whenever the device boots.', 'Die ValueBox startet bei jedem Gerätestart mit diesem Wert.', 'El ValueBox comienza con este valor cada vez que se inicia el dispositivo.', 'يبدأ ValueBox بهذه القيمة عند كل تشغيل للجهاز.'],
    ['Veri kaynağı', 'Data source', 'Datenquelle', 'Fuente de datos', 'مصدر البيانات'],
    ['Dijital giriş', 'Digital input', 'Digitaleingang', 'Entrada digital', 'دخل رقمي'],
    ['Slave ID ve seri iletişim seçenekleri Haberleşme Ayarları bölümünden değiştirilebilir. IEEE-754 float seçilen adresten başlayarak iki register kullanır (yüksek word önce).', 'The Slave ID and serial communication options can be changed under Communication Settings. IEEE-754 float uses two registers starting at the selected address (high word first).', 'Slave-ID und serielle Kommunikationsoptionen können unter Kommunikationseinstellungen geändert werden. IEEE-754 Float belegt ab der gewählten Adresse zwei Register (High-Word zuerst).', 'El ID de esclavo y las opciones de comunicación serie pueden cambiarse en Ajustes de comunicación. IEEE-754 float usa dos registros desde la dirección seleccionada (palabra alta primero).', 'يمكن تغيير معرّف التابع وخيارات الاتصال التسلسلي من قسم إعدادات الاتصال. تستخدم قيمة IEEE-754 العشرية سجلين بدءاً من العنوان المحدد (الكلمة العليا أولاً).'],
    ['Register adresi', 'Register address', 'Registeradresse', 'Dirección del registro', 'عنوان السجل'],
    ['Değer tipi', 'Value type', 'Werttyp', 'Tipo de valor', 'نوع القيمة'],
    ['Sayaç yönü', 'Counter direction', 'Zählrichtung', 'Dirección del contador', 'اتجاه العداد'],
    ['Yukarı sayıcı', 'Count up', 'Aufwärts zählen', 'Cuenta ascendente', 'عد تصاعدي'],
    ['Aşağı sayıcı', 'Count down', 'Abwärts zählen', 'Cuenta descendente', 'عد تنازلي'],
    ['IO12 dahili pull-down ile LOW seviyesinde bekler. Debounce sonrasındaki LOW → HIGH yükselen kenarında değer artırılır veya azaltılır. Negatif değerler gösterilebilir.', 'IO12 waits at LOW with its internal pull-down. The value increments or decrements on a debounced LOW → HIGH rising edge. Negative values can be displayed.', 'IO12 wartet mit internem Pull-down auf LOW. Nach dem Entprellen wird der Wert bei einer LOW → HIGH-Flanke erhöht oder verringert. Negative Werte sind möglich.', 'IO12 espera en LOW con su pull-down interno. Tras el antirrebote, el valor aumenta o disminuye en el flanco LOW → HIGH. Se admiten valores negativos.', 'ينتظر IO12 عند المستوى LOW باستخدام مقاومة السحب لأسفل الداخلية. بعد إزالة الارتداد تزيد أو تنقص القيمة عند الحافة الصاعدة LOW ← HIGH. يمكن عرض القيم السالبة.'],
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
    ['Sıcaklık Sensörü', 'Temperature Sensor', 'Temperatursensor', 'Sensor de temperatura', 'مستشعر درجة الحرارة'],
    ['Karta takılı sensör modelini seçin. Ayarlar kaydedildiğinde cihaz yeniden başlatılmadan uygulanır. Seçili sensör bulunamazsa panelde TMP?, SHT? veya HDC? gösterilir.', 'Select the sensor model fitted to the board. Settings are applied when saved, without a restart. If the selected sensor is not found, the panel shows TMP?, SHT? or HDC?.', 'Wählen Sie das auf der Platine bestückte Sensormodell. Die Einstellungen werden beim Speichern ohne Neustart übernommen. Wird der gewählte Sensor nicht gefunden, zeigt das Panel TMP?, SHT? oder HDC?.', 'Seleccione el modelo de sensor montado en la placa. Los ajustes se aplican al guardar, sin reiniciar. Si no se encuentra el sensor seleccionado, el panel muestra TMP?, SHT? o HDC?.', 'اختر طراز المستشعر المركب على اللوحة. تُطبَّق الإعدادات عند الحفظ دون إعادة تشغيل. إذا لم يُعثر على المستشعر المحدد تعرض اللوحة TMP? أو SHT? أو HDC?.'],
    ['Sensör durumu alınıyor', 'Getting sensor status', 'Sensorstatus wird abgerufen', 'Obteniendo estado del sensor', 'جارٍ جلب حالة المستشعر'],
    ['Sensör modeli', 'Sensor model', 'Sensormodell', 'Modelo de sensor', 'طراز المستشعر'],
    ['TMP112 — yalnızca sıcaklık', 'TMP112 — temperature only', 'TMP112 — nur Temperatur', 'TMP112 — solo temperatura', 'TMP112 — درجة الحرارة فقط'],
    ['SHT40 — sıcaklık ve nem', 'SHT40 — temperature and humidity', 'SHT40 — Temperatur und Feuchte', 'SHT40 — temperatura y humedad', 'SHT40 — درجة الحرارة والرطوبة'],
    ['HDC2022 — sıcaklık ve nem', 'HDC2022 — temperature and humidity', 'HDC2022 — Temperatur und Feuchte', 'HDC2022 — temperatura y humedad', 'HDC2022 — درجة الحرارة والرطوبة'],
    ['Ölçüm aralığı (saniye)', 'Measurement interval (seconds)', 'Messintervall (Sekunden)', 'Intervalo de medición (segundos)', 'فترة القياس (ثانية)'],
    ['Sıcaklık düzeltmesi (°C)', 'Temperature offset (°C)', 'Temperaturkorrektur (°C)', 'Corrección de temperatura (°C)', 'تصحيح درجة الحرارة (°C)'],
    ['Nem düzeltmesi (%RH)', 'Humidity offset (%RH)', 'Feuchtekorrektur (%RH)', 'Corrección de humedad (%RH)', 'تصحيح الرطوبة (%RH)'],
    ['Düzeltme değerleri ölçülen değere eklenir. Sensörü güvenilir bir referans ölçerle karşılaştırarak ayarlayın; sensör modeli değiştiğinde düzeltmeyi yeniden kontrol edin.', 'Offsets are added to the measured value. Set them by comparing the sensor with a trusted reference; check them again when the sensor model changes.', 'Korrekturwerte werden zum Messwert addiert. Stellen Sie sie durch Vergleich mit einem zuverlässigen Referenzgerät ein und prüfen Sie sie bei einem Sensorwechsel erneut.', 'Las correcciones se suman al valor medido. Ajústelas comparando el sensor con una referencia fiable y revíselas al cambiar de modelo de sensor.', 'تُضاف قيم التصحيح إلى القيمة المقيسة. اضبطها بمقارنة المستشعر بمرجع موثوق، وأعد التحقق منها عند تغيير طراز المستشعر.'],
    ['SHT40 ayarları', 'SHT40 settings', 'SHT40-Einstellungen', 'Ajustes de SHT40', 'إعدادات SHT40'],
    ['I2C adresi', 'I2C address', 'I2C-Adresse', 'Dirección I2C', 'عنوان I2C'],
    ['Ölçüm hassasiyeti', 'Measurement precision', 'Messgenauigkeit', 'Precisión de medición', 'دقة القياس'],
    ['Yüksek — en fazla 8.2 ms', 'High — up to 8.2 ms', 'Hoch — bis 8,2 ms', 'Alta — hasta 8,2 ms', 'عالية — حتى 8.2 ms'],
    ['Orta — en fazla 4.5 ms', 'Medium — up to 4.5 ms', 'Mittel — bis 4,5 ms', 'Media — hasta 4,5 ms', 'متوسطة — حتى 4.5 ms'],
    ['Düşük — en fazla 1.7 ms', 'Low — up to 1.7 ms', 'Niedrig — bis 1,7 ms', 'Baja — hasta 1,7 ms', 'منخفضة — حتى 1.7 ms'],
    ['Yüksek hassasiyet en düşük ölçüm gürültüsünü verir; düşük hassasiyet ölçümü kısaltır ve daha az enerji harcar. Adres, ürün kodundaki AD1B/BD1B sürümüne göre sabittir.', 'High precision gives the lowest measurement noise; low precision shortens the measurement and uses less energy. The address is fixed by the AD1B/BD1B variant in the part number.', 'Hohe Genauigkeit liefert das geringste Messrauschen; niedrige Genauigkeit verkürzt die Messung und spart Energie. Die Adresse ist durch die Variante AD1B/BD1B in der Teilenummer festgelegt.', 'La precisión alta ofrece el menor ruido de medición; la baja acorta la medición y consume menos energía. La dirección la fija la variante AD1B/BD1B del código de pieza.', 'تعطي الدقة العالية أقل ضوضاء قياس، بينما تقصّر الدقة المنخفضة زمن القياس وتستهلك طاقة أقل. يحدد إصدار AD1B/BD1B في رقم القطعة العنوان بشكل ثابت.'],
    ['TMP112 ayarları', 'TMP112 settings', 'TMP112-Einstellungen', 'Ajustes de TMP112', 'إعدادات TMP112'],
    ['Dönüşüm hızı', 'Conversion rate', 'Wandlungsrate', 'Velocidad de conversión', 'معدل التحويل'],
    ['4 Hz (varsayılan)', '4 Hz (default)', '4 Hz (Standard)', '4 Hz (predeterminado)', '4 Hz (افتراضي)'],
    ["Genişletilmiş ölçüm aralığı (13 bit, 150 °C'ye kadar)", 'Extended range (13 bit, up to 150 °C)', 'Erweiterter Messbereich (13 Bit, bis 150 °C)', 'Rango ampliado (13 bits, hasta 150 °C)', 'نطاق قياس موسّع (13 بت، حتى 150 °C)'],
    ['TMP112 yalnızca sıcaklık ölçer; nem gösteren kutular bu sensörle kullanılamaz. Sensör sürekli ölçüm yapar; düşük dönüşüm hızı akım tüketimini azaltır.', 'The TMP112 measures temperature only; boxes that show humidity cannot be used with this sensor. The sensor converts continuously; a lower conversion rate reduces current consumption.', 'Der TMP112 misst nur die Temperatur; Boxen mit Feuchteanzeige sind mit diesem Sensor nicht nutzbar. Der Sensor misst kontinuierlich; eine niedrigere Wandlungsrate senkt die Stromaufnahme.', 'El TMP112 solo mide la temperatura; los cuadros que muestran humedad no pueden usarse con este sensor. El sensor convierte de forma continua; una velocidad menor reduce el consumo.', 'يقيس TMP112 درجة الحرارة فقط، ولا يمكن استخدام المربعات التي تعرض الرطوبة مع هذا المستشعر. يقيس المستشعر باستمرار، ويقلل معدل التحويل المنخفض استهلاك التيار.'],
    ['HDC2022 ayarları', 'HDC2022 settings', 'HDC2022-Einstellungen', 'Ajustes de HDC2022', 'إعدادات HDC2022'],
    ['Sıcaklık çözünürlüğü', 'Temperature resolution', 'Temperaturauflösung', 'Resolución de temperatura', 'دقة تحليل درجة الحرارة'],
    ['Nem çözünürlüğü', 'Humidity resolution', 'Feuchteauflösung', 'Resolución de humedad', 'دقة تحليل الرطوبة'],
    ["ADDR pini boşta bırakılırsa adres 0x40 olur; gürültülü ortamlarda pini GND veya VDD'ye bağlayın. Ölçüm her aralıkta tek seferlik tetiklenir.", 'If the ADDR pin is left floating the address is 0x40; in noisy environments tie it to GND or VDD. A single measurement is triggered at every interval.', 'Bleibt der ADDR-Pin offen, ist die Adresse 0x40; in störungsreichen Umgebungen den Pin an GND oder VDD legen. Pro Intervall wird eine Einzelmessung ausgelöst.', 'Si el pin ADDR queda al aire, la dirección es 0x40; en entornos ruidosos conéctelo a GND o VDD. En cada intervalo se dispara una única medición.', 'إذا تُرك الطرف ADDR دون توصيل يكون العنوان 0x40؛ وفي البيئات المشوشة صِله بـ GND أو VDD. تُطلق قياسة واحدة في كل فترة.'],
    ['Sensör Ayarlarını Kaydet', 'Save Sensor Settings', 'Sensoreinstellungen speichern', 'Guardar ajustes del sensor', 'حفظ إعدادات المستشعر'],
    ['{sensor} bulunamadı', '{sensor} not found', '{sensor} nicht gefunden', '{sensor} no encontrado', 'لم يتم العثور على {sensor}'],
    ['{address} adresinde cevap yok; panelde {text} gösteriliyor. Bağlantıyı ve adres seçimini kontrol edin.', 'No answer at {address}; the panel shows {text}. Check the wiring and the address selection.', 'Keine Antwort an {address}; das Panel zeigt {text}. Prüfen Sie Verdrahtung und Adresswahl.', 'Sin respuesta en {address}; el panel muestra {text}. Compruebe el cableado y la dirección seleccionada.', 'لا يوجد رد على العنوان {address}؛ تعرض اللوحة {text}. تحقق من التوصيلات واختيار العنوان.'],
    ['{sensor} bulundu', '{sensor} found', '{sensor} gefunden', '{sensor} encontrado', 'تم العثور على {sensor}'],
    ['{address} adresinden ölçüm alınıyor.', 'Reading measurements from {address}.', 'Messwerte werden von {address} gelesen.', 'Leyendo mediciones desde {address}.', 'تُقرأ القياسات من العنوان {address}.'],
    ['İlk ölçüm bekleniyor…', 'Waiting for the first measurement…', 'Warte auf die erste Messung…', 'Esperando la primera medición…', 'في انتظار القياس الأول…'],
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
    ['Slave ID', 'Slave ID', 'Slave-ID', 'ID de esclavo', 'معرّف التابع'],
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

    // Editor: display area, ValueBox format and panel character set
    ['Türkçe, Almanca ve İspanyolca harfler ile ° işareti panelde gösterilir; ß “ss” olarak yazılır. Diğer karakterler “?” olarak görünür.', 'Turkish, German and Spanish letters and the ° sign are shown on the panel; ß is written as “ss”. Other characters appear as “?”.', 'Türkische, deutsche und spanische Buchstaben sowie das °-Zeichen werden auf dem Panel angezeigt; ß wird als „ss“ geschrieben. Andere Zeichen erscheinen als „?“.', 'Las letras turcas, alemanas y españolas y el signo ° se muestran en el panel; ß se escribe como «ss». Los demás caracteres aparecen como «?».', 'تُعرض الأحرف التركية والألمانية والإسبانية وعلامة ° على اللوحة، ويُكتب ß بالشكل “ss”. تظهر الأحرف الأخرى، ومنها الأحرف العربية، بالشكل “?”.'],
    ['Cihaz her açıldığında ValueBox bu değerden başlar. Tasarım kaydedildiğinde veri kaynağı ve başlangıç değeri değişmeyen kutular mevcut değerini korur.', 'The ValueBox starts from this value every time the device starts. When the design is saved, boxes whose data source and initial value are unchanged keep their current value.', 'Die ValueBox beginnt bei jedem Gerätestart mit diesem Wert. Beim Speichern des Designs behalten Boxen mit unveränderter Datenquelle und unverändertem Startwert ihren aktuellen Wert.', 'El ValueBox comienza con este valor cada vez que se inicia el dispositivo. Al guardar el diseño, los cuadros cuya fuente de datos y valor inicial no cambian conservan su valor actual.', 'يبدأ ValueBox من هذه القيمة في كل مرة يبدأ فيها الجهاز. عند حفظ التصميم تحتفظ المربعات التي لم يتغير مصدر بياناتها وقيمتها الابتدائية بقيمتها الحالية.'],
    ['Ondalık basamak', 'Decimal places', 'Nachkommastellen', 'Decimales', 'المنازل العشرية'],
    ['Birim / son ek', 'Unit / suffix', 'Einheit / Suffix', 'Unidad / sufijo', 'الوحدة / اللاحقة'],
    ['Float değer bu basamak sayısına yuvarlanır. Integer değerde ondalık nokta kaydırılır; örneğin 1234 ve 2 basamak 12.34 olarak gösterilir.', 'A float value is rounded to this many decimals. For an integer value the decimal point is shifted; for example, 1234 with 2 decimals is shown as 12.34.', 'Ein Float-Wert wird auf diese Anzahl Nachkommastellen gerundet. Bei einem Integer-Wert wird der Dezimalpunkt verschoben; zum Beispiel wird 1234 mit 2 Stellen als 12.34 angezeigt.', 'Un valor float se redondea a estos decimales. En un valor entero se desplaza el punto decimal; por ejemplo, 1234 con 2 decimales se muestra como 12.34.', 'تُقرَّب قيمة float إلى هذا العدد من المنازل العشرية. في القيمة الصحيحة تُزاح الفاصلة العشرية؛ فمثلًا تُعرض 1234 مع منزلتين بالشكل 12.34.'],
    ['Gösterim alanı', 'Display area', 'Anzeigebereich', 'Área de visualización', 'منطقة العرض'],
    ['Yalnızca kendi paneli', 'Own panel only', 'Nur eigenes Panel', 'Solo su propio panel', 'لوحته فقط'],
    ['Tüm ekran', 'Whole display', 'Gesamte Anzeige', 'Toda la pantalla', 'الشاشة بالكامل'],
    ['Kutu bu alanın dışında kırpılır; kayar yazı bu alanın kenarlarında döner.', 'The box is clipped outside this area; scrolling text wraps around at its edges.', 'Die Box wird außerhalb dieses Bereichs abgeschnitten; Lauftext springt an dessen Rändern um.', 'El cuadro se recorta fuera de esta área; el texto desplazable se reinicia en sus bordes.', 'يُقص المربع خارج هذه المنطقة، ويلتف النص المتحرك عند حوافها.'],
    ['Ondalık basamak sayısı değişti.', 'Decimal places changed.', 'Nachkommastellen geändert.', 'Decimales cambiados.', 'تم تغيير عدد المنازل العشرية.'],
    ['Gösterim alanı değişti.', 'Display area changed.', 'Anzeigebereich geändert.', 'Área de visualización cambiada.', 'تم تغيير منطقة العرض.'],

    // Settings: brightness, clock, NTP and Wi-Fi
    ['Yalnızca parlaklık değiştiyse ayar yeniden başlatmadan hemen uygulanır.', 'If only the brightness changed, it is applied immediately without a restart.', 'Wenn nur die Helligkeit geändert wurde, wird sie sofort ohne Neustart übernommen.', 'Si solo cambió el brillo, se aplica de inmediato sin reiniciar.', 'إذا تغيّر السطوع فقط فسيُطبَّق فورًا دون إعادة تشغيل.'],
    ['RTC bulunamadı. DS3231 bağlantısını kontrol edin; saat ve tarih ayarlanamaz.', 'RTC not found. Check the DS3231 connection; time and date cannot be set.', 'RTC nicht gefunden. Prüfen Sie den DS3231-Anschluss; Uhrzeit und Datum können nicht eingestellt werden.', 'No se encontró el RTC. Compruebe la conexión del DS3231; no se pueden ajustar la hora ni la fecha.', 'لم يتم العثور على RTC. تحقق من توصيل DS3231؛ لا يمكن ضبط الوقت والتاريخ.'],
    ['Alanlar cihaz saatiyle birlikte ilerler. Elle girdiğiniz değer de kaydedene kadar saymaya devam eder.', 'The fields run along with the device clock. A value you enter manually keeps counting until you save it.', 'Die Felder laufen mit der Geräteuhr mit. Ein manuell eingegebener Wert läuft bis zum Speichern weiter.', 'Los campos avanzan con el reloj del dispositivo. Un valor introducido manualmente sigue contando hasta que lo guarde.', 'تتقدم الحقول مع ساعة الجهاز، وتستمر القيمة التي تُدخلها يدويًا في العد حتى تحفظها.'],
    ['İnternet saati (NTP)', 'Internet time (NTP)', 'Internetzeit (NTP)', 'Hora de Internet (NTP)', 'وقت الإنترنت (NTP)'],
    ['STA ağına bağlıyken saati otomatik eşitle', 'Synchronize the clock automatically while connected to the STA network', 'Uhr automatisch synchronisieren, solange das STA-Netz verbunden ist', 'Sincronizar el reloj automáticamente mientras esté conectado a la red STA', 'مزامنة الساعة تلقائيًا أثناء الاتصال بشبكة STA'],
    ['NTP sunucusu', 'NTP server', 'NTP-Server', 'Servidor NTP', 'خادم NTP'],
    ['Saat dilimi', 'Time zone', 'Zeitzone', 'Zona horaria', 'المنطقة الزمنية'],
    ['UTC+3 — Türkiye, Suudi Arabistan', 'UTC+3 — Türkiye, Saudi Arabia', 'UTC+3 — Türkei, Saudi-Arabien', 'UTC+3 — Turquía, Arabia Saudí', 'UTC+3 — تركيا، السعودية'],
    ['Birleşik Krallık (UTC+0, yaz saati)', 'United Kingdom (UTC+0, DST)', 'Vereinigtes Königreich (UTC+0, Sommerzeit)', 'Reino Unido (UTC+0, horario de verano)', 'المملكة المتحدة (UTC+0، توقيت صيفي)'],
    ['Orta Avrupa — Almanya, İspanya (UTC+1, yaz saati)', 'Central Europe — Germany, Spain (UTC+1, DST)', 'Mitteleuropa — Deutschland, Spanien (UTC+1, Sommerzeit)', 'Europa Central — Alemania, España (UTC+1, horario de verano)', 'وسط أوروبا — ألمانيا، إسبانيا (UTC+1، توقيت صيفي)'],
    ['Doğu Avrupa — Yunanistan (UTC+2, yaz saati)', 'Eastern Europe — Greece (UTC+2, DST)', 'Osteuropa — Griechenland (UTC+2, Sommerzeit)', 'Europa del Este — Grecia (UTC+2, horario de verano)', 'شرق أوروبا — اليونان (UTC+2، توقيت صيفي)'],
    ['Moskova (UTC+3)', 'Moscow (UTC+3)', 'Moskau (UTC+3)', 'Moscú (UTC+3)', 'موسكو (UTC+3)'],
    ['Körfez — BAE (UTC+4)', 'Gulf — UAE (UTC+4)', 'Golf — VAE (UTC+4)', 'Golfo — EAU (UTC+4)', 'الخليج — الإمارات (UTC+4)'],
    ['ABD Doğu (UTC−5, yaz saati)', 'US Eastern (UTC−5, DST)', 'USA Ost (UTC−5, Sommerzeit)', 'EE. UU. Este (UTC−5, horario de verano)', 'شرق الولايات المتحدة (UTC−5، توقيت صيفي)'],
    ['Özel POSIX TZ değeri', 'Custom POSIX TZ value', 'Eigener POSIX-TZ-Wert', 'Valor POSIX TZ personalizado', 'قيمة POSIX TZ مخصصة'],
    ['POSIX TZ değeri', 'POSIX TZ value', 'POSIX-TZ-Wert', 'Valor POSIX TZ', 'قيمة POSIX TZ'],
    ["Eşitlenen yerel saat RTC'ye yazılır; yaz saati geçişleri seçilen saat dilimine göre uygulanır. NTP açıkken elle girilen saat bir sonraki eşitlemede düzeltilir.", 'The synchronized local time is written to the RTC; daylight saving changes follow the selected time zone. While NTP is on, a manually set time is corrected at the next synchronization.', 'Die synchronisierte Ortszeit wird in die RTC geschrieben; Sommerzeitwechsel folgen der gewählten Zeitzone. Bei aktivem NTP wird eine manuell eingestellte Zeit bei der nächsten Synchronisierung korrigiert.', 'La hora local sincronizada se escribe en el RTC; los cambios de horario de verano siguen la zona horaria seleccionada. Con NTP activado, una hora ajustada manualmente se corrige en la siguiente sincronización.', 'يُكتب الوقت المحلي المُزامَن في RTC، وتتبع تغييرات التوقيت الصيفي المنطقة الزمنية المحددة. عند تفعيل NTP يُصحَّح الوقت المضبوط يدويًا في المزامنة التالية.'],
    ['NTP Ayarlarını Kaydet', 'Save NTP Settings', 'NTP-Einstellungen speichern', 'Guardar ajustes NTP', 'حفظ إعدادات NTP'],
    ['Uyku Ayarlarını Kaydet', 'Save Sleep Settings', 'Ruhezustand speichern', 'Guardar ajustes de reposo', 'حفظ إعدادات السكون'],
    ['NTP eşitlemesi kapalı.', 'NTP synchronization is off.', 'NTP-Synchronisierung ist aus.', 'La sincronización NTP está desactivada.', 'مزامنة NTP متوقفة.'],
    ['Saat az önce NTP ile eşitlendi.', 'The clock was just synchronized via NTP.', 'Die Uhr wurde gerade per NTP synchronisiert.', 'El reloj se acaba de sincronizar mediante NTP.', 'تمت مزامنة الساعة عبر NTP للتو.'],
    ['Son NTP eşitlemesi {minutes} dakika önce yapıldı.', 'Last NTP synchronization was {minutes} minutes ago.', 'Letzte NTP-Synchronisierung vor {minutes} Minuten.', 'La última sincronización NTP fue hace {minutes} minutos.', 'آخر مزامنة NTP كانت قبل {minutes} دقيقة.'],
    ['NTP sunucusundan yanıt bekleniyor…', 'Waiting for a response from the NTP server…', 'Warte auf Antwort des NTP-Servers…', 'Esperando respuesta del servidor NTP…', 'في انتظار رد من خادم NTP…'],
    ['NTP için cihazın STA ağına bağlı olması gerekir.', 'NTP requires the device to be connected to the STA network.', 'Für NTP muss das Gerät mit dem STA-Netz verbunden sein.', 'NTP requiere que el dispositivo esté conectado a la red STA.', 'يتطلب NTP أن يكون الجهاز متصلًا بشبكة STA.'],
    ['Varsayılan AP parolası kullanılıyor. Cihazı devreye almadan önce parolayı değiştirin.', 'The default AP password is in use. Change it before putting the device into service.', 'Das Standard-AP-Passwort wird verwendet. Ändern Sie es, bevor Sie das Gerät in Betrieb nehmen.', 'Se está usando la contraseña predeterminada del AP. Cámbiela antes de poner el dispositivo en servicio.', 'كلمة مرور نقطة الوصول الافتراضية قيد الاستخدام. غيّرها قبل تشغيل الجهاز فعليًا.'],
    ['Kayıtlı parolalar güvenlik nedeniyle bu sayfada gösterilmez; alan boş bırakılırsa mevcut parola korunur.', 'Stored passwords are not shown on this page for security reasons; leaving the field empty keeps the current password.', 'Gespeicherte Passwörter werden aus Sicherheitsgründen auf dieser Seite nicht angezeigt; ein leeres Feld behält das aktuelle Passwort bei.', 'Por seguridad, las contraseñas guardadas no se muestran en esta página; si deja el campo vacío se conserva la contraseña actual.', 'لا تُعرض كلمات المرور المحفوظة في هذه الصفحة لأسباب أمنية؛ وإذا تُرك الحقل فارغًا تبقى كلمة المرور الحالية.'],
    ['Bu ağdan erişimde kullanıcı adı ve parola iste', 'Require user name and password for access from this network', 'Benutzername und Passwort für den Zugriff aus diesem Netzwerk verlangen', 'Pedir usuario y contraseña para acceder desde esta red', 'طلب اسم المستخدم وكلمة المرور للوصول من هذه الشبكة'],
    ['Açıkken bu ağdan bağlanan tarayıcı giriş ister: kullanıcı adı admin, parola cihazın AP parolasıdır. Portal HTTPS kullanmadığı için Chrome bu girişte bağlantının gizli olmadığı uyarısını gösterir. Kapalıyken aynı ağdaki herkes ayarları değiştirebilir ve firmware yükleyebilir.', 'When on, a browser connecting from this network must log in: the user name is admin and the password is the device AP password. Because the portal does not use HTTPS, Chrome shows a “connection is not private” warning at this login. When off, anyone on the same network can change settings and upload firmware.', 'Wenn aktiviert, muss sich ein Browser aus diesem Netzwerk anmelden: Benutzername admin, Passwort ist das AP-Passwort des Geräts. Da das Portal kein HTTPS verwendet, zeigt Chrome bei dieser Anmeldung den Hinweis „Verbindung nicht privat“. Wenn deaktiviert, kann jeder im selben Netzwerk Einstellungen ändern und Firmware hochladen.', 'Si está activado, el navegador que se conecte desde esta red debe iniciar sesión: el usuario es admin y la contraseña es la del AP del dispositivo. Como el portal no usa HTTPS, Chrome muestra en este inicio de sesión el aviso de que la conexión no es privada. Si está desactivado, cualquiera en la misma red puede cambiar los ajustes y cargar firmware.', 'عند التفعيل يطلب المتصفح المتصل من هذه الشبكة تسجيل الدخول: اسم المستخدم admin وكلمة المرور هي كلمة مرور نقطة الوصول للجهاز. ولأن البوابة لا تستخدم HTTPS يعرض Chrome عند هذا الدخول تحذيرًا بأن الاتصال غير خاص. عند الإيقاف يستطيع أي شخص على الشبكة نفسها تغيير الإعدادات ورفع البرنامج.'],
    ['Kayıtlı parola korunur; değiştirmek için yazın', 'The stored password is kept; type to change it', 'Das gespeicherte Passwort bleibt erhalten; zum Ändern eingeben', 'Se conserva la contraseña guardada; escriba para cambiarla', 'تُحفظ كلمة المرور الحالية؛ اكتب لتغييرها'],

    // Device messages (UI_TEXT in the firmware sources)
    ['Tarama oranı 1/4 veya 1/8 olmalıdır.', 'Scan rate must be 1/4 or 1/8.', 'Die Scanrate muss 1/4 oder 1/8 sein.', 'La tasa de escaneo debe ser 1/4 o 1/8.', 'يجب أن يكون معدل المسح 1/4 أو 1/8.'],
    ['Satır sayısı geçersiz.', 'Number of rows is invalid.', 'Die Anzahl der Zeilen ist ungültig.', 'El número de filas no es válido.', 'عدد الصفوف غير صالح.'],
    ['Sütun sayısı geçersiz.', 'Number of columns is invalid.', 'Die Anzahl der Spalten ist ungültig.', 'El número de columnas no es válido.', 'عدد الأعمدة غير صالح.'],
    ['Toplam panel sayısı izin verilen sınırı aşıyor.', 'The total number of panels exceeds the allowed limit.', 'Die Gesamtzahl der Panels überschreitet das zulässige Limit.', 'El número total de paneles supera el límite permitido.', 'العدد الإجمالي للوحات يتجاوز الحد المسموح به.'],
    ['Panel bağlantı şekli geçersiz.', 'Panel connection pattern is invalid.', 'Das Panel-Verbindungsmuster ist ungültig.', 'El patrón de conexión del panel no es válido.', 'نمط توصيل اللوحة غير صالح.'],
    ['AP SSID 1-32 karakter olmalıdır.', 'AP SSID must be 1-32 characters.', 'Die AP-SSID muss 1-32 Zeichen lang sein.', 'El SSID del AP debe tener entre 1 y 32 caracteres.', 'يجب أن يتكون SSID لنقطة الوصول من 1 إلى 32 حرفًا.'],
    ['AP parolası 8-63 karakter olmalıdır.', 'AP password must be 8-63 characters.', 'Das AP-Passwort muss 8-63 Zeichen lang sein.', 'La contraseña del AP debe tener entre 8 y 63 caracteres.', 'يجب أن تتكون كلمة مرور نقطة الوصول من 8 إلى 63 حرفًا.'],
    ['STA SSID en fazla 32 karakter olabilir.', 'STA SSID can be at most 32 characters.', 'Die STA-SSID darf höchstens 32 Zeichen lang sein.', 'El SSID de STA puede tener como máximo 32 caracteres.', 'يمكن أن يتكون SSID لشبكة STA من 32 حرفًا كحد أقصى.'],
    ['STA parolası boş veya 8-63 karakter olmalıdır.', 'STA password must be empty or 8-63 characters.', 'Das STA-Passwort muss leer oder 8-63 Zeichen lang sein.', 'La contraseña de STA debe estar vacía o tener entre 8 y 63 caracteres.', 'يجب أن تكون كلمة مرور STA فارغة أو من 8 إلى 63 حرفًا.'],
    ['STA SSID boşken parola girilemez.', 'A password cannot be entered while the STA SSID is empty.', 'Ohne STA-SSID kann kein Passwort eingegeben werden.', 'No se puede introducir una contraseña si el SSID de STA está vacío.', 'لا يمكن إدخال كلمة مرور بينما SSID لشبكة STA فارغ.'],
    ['Modbus Slave ID 1-247 aralığında olmalıdır.', 'Modbus slave ID must be between 1 and 247.', 'Die Modbus-Slave-ID muss zwischen 1 und 247 liegen.', 'El ID de esclavo Modbus debe estar entre 1 y 247.', 'يجب أن يكون معرّف Modbus التابع بين 1 و247.'],
    ['UART baudrate geçersiz.', 'UART baud rate is invalid.', 'Die UART-Baudrate ist ungültig.', 'La velocidad en baudios del UART no es válida.', 'معدل باود UART غير صالح.'],
    ['UART data size 5-8 bit olmalıdır.', 'UART data size must be 5-8 bits.', 'Die UART-Datengröße muss 5-8 Bit betragen.', 'El tamaño de datos del UART debe ser de 5 a 8 bits.', 'يجب أن يكون حجم بيانات UART من 5 إلى 8 بت.'],
    ['UART parity ayarı geçersiz.', 'UART parity setting is invalid.', 'Die UART-Paritätseinstellung ist ungültig.', 'El ajuste de paridad del UART no es válido.', 'إعداد تماثل UART غير صالح.'],
    ['UART handshake ayarı geçersiz.', 'UART handshake setting is invalid.', 'Die UART-Handshake-Einstellung ist ungültig.', 'El ajuste de handshake del UART no es válido.', 'إعداد مصافحة UART غير صالح.'],
    ['Uyku başlangıç veya bitiş saati geçersiz.', 'Sleep start or end time is invalid.', 'Beginn oder Ende des Ruhezustands ist ungültig.', 'La hora de inicio o fin del reposo no es válida.', 'وقت بدء السكون أو انتهائه غير صالح.'],
    ['Uyku başlangıç ve bitiş saati farklı olmalıdır.', 'Sleep start and end times must be different.', 'Beginn und Ende des Ruhezustands müssen sich unterscheiden.', 'Las horas de inicio y fin del reposo deben ser distintas.', 'يجب أن يختلف وقت بدء السكون عن وقت انتهائه.'],
    ['Uyku açılış günü geçersiz.', 'Panel wake-up day setting is invalid.', 'Die Einschalttage des Panels sind ungültig.', 'El ajuste de días de encendido del panel no es válido.', 'إعداد أيام تشغيل اللوحة غير صالح.'],
    ['NTP sunucusu 1-63 karakter olmalı; yalnızca harf, rakam, nokta, tire ve iki nokta içerebilir.', 'The NTP server must be 1-63 characters and may only contain letters, digits, dots, hyphens and colons.', 'Der NTP-Server muss 1-63 Zeichen lang sein und darf nur Buchstaben, Ziffern, Punkte, Bindestriche und Doppelpunkte enthalten.', 'El servidor NTP debe tener entre 1 y 63 caracteres y solo puede contener letras, dígitos, puntos, guiones y dos puntos.', 'يجب أن يتكون خادم NTP من 1 إلى 63 حرفًا وأن يحتوي فقط على أحرف وأرقام ونقاط وشرطات ونقطتين رأسيتين.'],
    ['Saat dilimi 1-63 karakterlik geçerli bir POSIX TZ değeri olmalıdır.', 'The time zone must be a valid POSIX TZ value of 1-63 characters.', 'Die Zeitzone muss ein gültiger POSIX-TZ-Wert mit 1-63 Zeichen sein.', 'La zona horaria debe ser un valor POSIX TZ válido de 1 a 63 caracteres.', 'يجب أن تكون المنطقة الزمنية قيمة POSIX TZ صالحة من 1 إلى 63 حرفًا.'],
    ['Alarm veri formatı geçersiz.', 'Alarm data format is invalid.', 'Das Alarm-Datenformat ist ungültig.', 'El formato de datos de alarma no es válido.', 'تنسيق بيانات المنبه غير صالح.'],
    ['Alarm saati geçersiz.', 'Alarm time is invalid.', 'Die Alarmzeit ist ungültig.', 'La hora de la alarma no es válida.', 'وقت المنبه غير صالح.'],
    ['Alarm tetiklenme süresi 1-3600 saniye olmalıdır.', 'Alarm duration must be 1-3600 seconds.', 'Die Alarmdauer muss 1-3600 Sekunden betragen.', 'La duración de la alarma debe ser de 1 a 3600 segundos.', 'يجب أن تكون مدة المنبه من 1 إلى 3600 ثانية.'],
    ['Alarm gün seçimi geçersiz.', 'Alarm day selection is invalid.', 'Die Tagesauswahl des Alarms ist ungültig.', 'La selección de días de la alarma no es válida.', 'اختيار أيام المنبه غير صالح.'],
    ['Alarm için en az bir geçerli gün seçilmelidir.', 'At least one valid day must be selected for the alarm.', 'Für den Alarm muss mindestens ein gültiger Tag ausgewählt werden.', 'Debe seleccionarse al menos un día válido para la alarma.', 'يجب اختيار يوم صالح واحد على الأقل للمنبه.'],
    ['İçerik formatı geçersiz.', 'Content format is invalid.', 'Das Inhaltsformat ist ungültig.', 'El formato del contenido no es válido.', 'تنسيق المحتوى غير صالح.'],
    ['İçerik kutusu tipi geçersiz.', 'Content box type is invalid.', 'Der Typ der Inhaltsbox ist ungültig.', 'El tipo de cuadro de contenido no es válido.', 'نوع مربع المحتوى غير صالح.'],
    ['İçerik kutusu panel numarası geçersiz.', 'Content box panel number is invalid.', 'Die Panelnummer der Inhaltsbox ist ungültig.', 'El número de panel del cuadro de contenido no es válido.', 'رقم لوحة مربع المحتوى غير صالح.'],
    ['Bir panelde toplam en fazla iki TextBox/ValueBox olabilir.', 'A panel can contain at most two TextBoxes/ValueBoxes in total.', 'Ein Panel kann insgesamt höchstens zwei TextBoxen/ValueBoxen enthalten.', 'Un panel puede contener como máximo dos TextBox/ValueBox en total.', 'يمكن أن تحتوي اللوحة على مربعين TextBox/ValueBox كحد أقصى.'],
    ['İçerik kutusu koordinatı ekran dışında.', 'Content box coordinate is outside the display.', 'Die Koordinate der Inhaltsbox liegt außerhalb der Anzeige.', 'La coordenada del cuadro de contenido está fuera de la pantalla.', 'إحداثي مربع المحتوى خارج الشاشة.'],
    ['İçerik kutusu koordinatı ile panel numarası uyuşmuyor.', 'Content box coordinate does not match its panel number.', 'Koordinate und Panelnummer der Inhaltsbox stimmen nicht überein.', 'La coordenada del cuadro de contenido no coincide con su número de panel.', 'إحداثي مربع المحتوى لا يطابق رقم اللوحة.'],
    ['İçerik kutusu döndürme açısı geçersiz.', 'Content box rotation angle is invalid.', 'Der Drehwinkel der Inhaltsbox ist ungültig.', 'El ángulo de rotación del cuadro de contenido no es válido.', 'زاوية تدوير مربع المحتوى غير صالحة.'],
    ['İçerik kutusu yazı tipi geçersiz.', 'Content box font is invalid.', 'Die Schriftart der Inhaltsbox ist ungültig.', 'La fuente del cuadro de contenido no es válida.', 'خط مربع المحتوى غير صالح.'],
    ['İçerik kutusu gösterim alanı geçersiz.', 'Content box display area is invalid.', 'Der Anzeigebereich der Inhaltsbox ist ungültig.', 'El área de visualización del cuadro de contenido no es válida.', 'منطقة عرض مربع المحتوى غير صالحة.'],
    ['İçerik kutusu metni sonlandırılmamış.', 'Content box text is not terminated.', 'Der Text der Inhaltsbox ist nicht abgeschlossen.', 'El texto del cuadro de contenido no está terminado.', 'نص مربع المحتوى غير منتهٍ.'],
    ['TextBox sıcaklık gösterim tipi geçersiz.', 'TextBox temperature display type is invalid.', 'Die Temperaturanzeige der TextBox ist ungültig.', 'El tipo de visualización de temperatura del TextBox no es válido.', 'نوع عرض درجة الحرارة في TextBox غير صالح.'],
    ['TextBox veri tipi geçersiz.', 'TextBox data type is invalid.', 'Der Datentyp der TextBox ist ungültig.', 'El tipo de datos del TextBox no es válido.', 'نوع بيانات TextBox غير صالح.'],
    ["Metin TextBox'u boş olamaz.", 'A text TextBox cannot be empty.', 'Eine Text-TextBox darf nicht leer sein.', 'Un TextBox de texto no puede estar vacío.', 'لا يمكن أن يكون TextBox النصي فارغًا.'],
    ['TextBox döngü içeriği geçersiz.', 'TextBox cycle content is invalid.', 'Der Wechselinhalt der TextBox ist ungültig.', 'El contenido de ciclo del TextBox no es válido.', 'محتوى التبديل في TextBox غير صالح.'],
    ['Döngü süresi 1-3600 saniye olmalıdır.', 'Cycle interval must be 1-3600 seconds.', 'Das Wechselintervall muss 1-3600 Sekunden betragen.', 'El intervalo de ciclo debe ser de 1 a 3600 segundos.', 'يجب أن تكون مدة التبديل من 1 إلى 3600 ثانية.'],
    ['Döngüdeki yazı içeriği boş olamaz.', 'The text in the cycle cannot be empty.', 'Der Text im Wechsel darf nicht leer sein.', 'El texto del ciclo no puede estar vacío.', 'لا يمكن أن يكون النص في التبديل فارغًا.'],
    ['ValueBox veri kaynağı geçersiz.', 'ValueBox data source is invalid.', 'Die Datenquelle der ValueBox ist ungültig.', 'La fuente de datos del ValueBox no es válida.', 'مصدر بيانات ValueBox غير صالح.'],
    ['ValueBox Modbus veri tipi geçersiz.', 'ValueBox Modbus data type is invalid.', 'Der Modbus-Datentyp der ValueBox ist ungültig.', 'El tipo de datos Modbus del ValueBox no es válido.', 'نوع بيانات Modbus في ValueBox غير صالح.'],
    ['ValueBox sayaç yönü geçersiz.', 'ValueBox counter direction is invalid.', 'Die Zählrichtung der ValueBox ist ungültig.', 'La dirección del contador del ValueBox no es válida.', 'اتجاه عداد ValueBox غير صالح.'],
    ['ValueBox başlangıç float değeri geçersiz.', 'ValueBox initial float value is invalid.', 'Der Float-Startwert der ValueBox ist ungültig.', 'El valor float inicial del ValueBox no es válido.', 'قيمة float الابتدائية في ValueBox غير صالحة.'],
    ['ValueBox başlangıç integer değeri geçersiz.', 'ValueBox initial integer value is invalid.', 'Der Integer-Startwert der ValueBox ist ungültig.', 'El valor entero inicial del ValueBox no es válido.', 'قيمة integer الابتدائية في ValueBox غير صالحة.'],
    ['ValueBox ondalık basamak sayısı 0-3 olmalıdır.', 'ValueBox decimal places must be 0-3.', 'Die Nachkommastellen der ValueBox müssen 0-3 sein.', 'Los decimales del ValueBox deben estar entre 0 y 3.', 'يجب أن يكون عدد المنازل العشرية في ValueBox من 0 إلى 3.'],
    ['ValueBox birimi en fazla 15 bayt olabilir.', 'The ValueBox unit can be at most 15 bytes.', 'Die Einheit der ValueBox darf höchstens 15 Byte lang sein.', 'La unidad del ValueBox puede tener como máximo 15 bytes.', 'يمكن أن تكون وحدة ValueBox بحد أقصى 15 بايت.'],
    ['IEEE-754 float için son register adresi kullanılamaz.', 'The last register address cannot be used for an IEEE-754 float.', 'Die letzte Registeradresse kann nicht für einen IEEE-754-Float verwendet werden.', 'La última dirección de registro no puede usarse para un float IEEE-754.', 'لا يمكن استخدام آخر عنوان سجل لقيمة IEEE-754 float.'],
    ['Modbus ValueBox register adresleri birbiriyle çakışmamalıdır.', 'Modbus ValueBox register addresses must not overlap.', 'Die Modbus-Registeradressen der ValueBoxen dürfen sich nicht überschneiden.', 'Las direcciones de registro Modbus de los ValueBox no deben solaparse.', 'يجب ألا تتداخل عناوين سجلات Modbus لمربعات ValueBox.'],
    ['Kaydırma yönü geçersiz.', 'Scroll direction is invalid.', 'Die Laufrichtung ist ungültig.', 'La dirección de desplazamiento no es válida.', 'اتجاه التمرير غير صالح.'],
    ['Kaydırma hızı 1-100 piksel/saniye olmalıdır.', 'Scroll speed must be 1-100 pixels/second.', 'Die Laufgeschwindigkeit muss 1-100 Pixel/Sekunde betragen.', 'La velocidad de desplazamiento debe ser de 1 a 100 píxeles/segundo.', 'يجب أن تكون سرعة التمرير من 1 إلى 100 بكسل/ثانية.'],
    ['Çok fazla TextBox/ValueBox gönderildi.', 'Too many TextBoxes/ValueBoxes were sent.', 'Es wurden zu viele TextBoxen/ValueBoxen gesendet.', 'Se enviaron demasiados TextBox/ValueBox.', 'تم إرسال عدد كبير جدًا من مربعات TextBox/ValueBox.'],
    ['İçerik kutusu veri formatı geçersiz.', 'Content box data format is invalid.', 'Das Datenformat der Inhaltsbox ist ungültig.', 'El formato de datos del cuadro de contenido no es válido.', 'تنسيق بيانات مربع المحتوى غير صالح.'],
    ['İçerik kutusu alanlarından biri geçersiz.', 'One of the content box fields is invalid.', 'Eines der Felder der Inhaltsbox ist ungültig.', 'Uno de los campos del cuadro de contenido no es válido.', 'أحد حقول مربع المحتوى غير صالح.'],
    ['TextBox döngü alanlarından biri geçersiz.', 'One of the TextBox cycle fields is invalid.', 'Eines der Wechselfelder der TextBox ist ungültig.', 'Uno de los campos de ciclo del TextBox no es válido.', 'أحد حقول التبديل في TextBox غير صالح.'],
    ['ValueBox alanlarından biri geçersiz.', 'One of the ValueBox fields is invalid.', 'Eines der ValueBox-Felder ist ungültig.', 'Uno de los campos del ValueBox no es válido.', 'أحد حقول ValueBox غير صالح.'],
    ['İçerik kutusu metni çok uzun veya geçersiz.', 'Content box text is too long or invalid.', 'Der Text der Inhaltsbox ist zu lang oder ungültig.', 'El texto del cuadro de contenido es demasiado largo o no es válido.', 'نص مربع المحتوى طويل جدًا أو غير صالح.'],
    ['Yazı tipi kimliği geçersiz.', 'Font ID is invalid.', 'Die Schriftart-ID ist ungültig.', 'El identificador de fuente no es válido.', 'معرّف الخط غير صالح.'],
    ['Yazı tipi önizleme verisi çok büyük.', 'Font preview data is too large.', 'Die Schriftvorschaudaten sind zu groß.', 'Los datos de vista previa de la fuente son demasiado grandes.', 'بيانات معاينة الخط كبيرة جدًا.'],
    ['firmware.bin dosyası gönderilmedi.', 'No firmware.bin file was sent.', 'Es wurde keine firmware.bin-Datei gesendet.', 'No se envió ningún archivo firmware.bin.', 'لم يتم إرسال ملف firmware.bin.'],
    ['Ekran içeriği veri sınırını aşıyor.', 'Display content exceeds the data limit.', 'Der Anzeigeinhalt überschreitet die Datengrenze.', 'El contenido de la pantalla supera el límite de datos.', 'محتوى الشاشة يتجاوز حد البيانات.'],
    ['Ekran tasarımı için bellek ayrılamadı.', 'Memory for the display design could not be allocated.', 'Für das Anzeigedesign konnte kein Speicher reserviert werden.', 'No se pudo asignar memoria para el diseño de pantalla.', 'تعذر حجز الذاكرة لتصميم الشاشة.'],
    ['Ekran tasarımı flash belleğe kaydedilemedi.', 'The display design could not be saved to flash.', 'Das Anzeigedesign konnte nicht im Flash gespeichert werden.', 'No se pudo guardar el diseño de pantalla en la memoria flash.', 'تعذر حفظ تصميم الشاشة في ذاكرة الفلاش.'],
    ['Ekran tasarımı panele uygulandı ve kaydedildi.', 'The display design was applied and saved.', 'Das Anzeigedesign wurde angewendet und gespeichert.', 'El diseño de pantalla se aplicó y se guardó.', 'تم تطبيق تصميم الشاشة وحفظه.'],
    ['Eksik veya geçersiz panel ayarı.', 'Missing or invalid panel setting.', 'Fehlende oder ungültige Panel-Einstellung.', 'Ajuste del panel ausente o no válido.', 'إعداد اللوحة مفقود أو غير صالح.'],
    ['Panel ayarları flash belleğe kaydedilemedi.', 'Panel settings could not be saved to flash.', 'Die Panel-Einstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes del panel en la memoria flash.', 'تعذر حفظ إعدادات اللوحة في ذاكرة الفلاش.'],
    ['Parlaklık kaydedildi ve panele uygulandı.', 'Brightness saved and applied to the panel.', 'Helligkeit gespeichert und auf das Panel angewendet.', 'Brillo guardado y aplicado al panel.', 'تم حفظ السطوع وتطبيقه على اللوحة.'],
    ['Panel ayarları kaydedildi. Cihaz yeniden başlatılacak.', 'Panel settings saved. The device will restart.', 'Panel-Einstellungen gespeichert. Das Gerät wird neu gestartet.', 'Ajustes del panel guardados. El dispositivo se reiniciará.', 'تم حفظ إعدادات اللوحة. سيعاد تشغيل الجهاز.'],
    ['Panel ayarları kaydedildi. Mevcut ekran tasarımı yeni yerleşime sığmadığı için yeniden başlatmada varsayılan tasarım yüklenecek.', 'Panel settings saved. The current display design does not fit the new layout, so the default design will be loaded after the restart.', 'Panel-Einstellungen gespeichert. Das aktuelle Anzeigedesign passt nicht zum neuen Layout; nach dem Neustart wird das Standarddesign geladen.', 'Ajustes del panel guardados. El diseño actual no cabe en la nueva disposición, por lo que se cargará el diseño predeterminado tras el reinicio.', 'تم حفظ إعدادات اللوحة. لا يتناسب تصميم الشاشة الحالي مع التخطيط الجديد، لذا سيُحمَّل التصميم الافتراضي بعد إعادة التشغيل.'],
    ['Tarih veya saat geçersiz.', 'Date or time is invalid.', 'Datum oder Uhrzeit ist ungültig.', 'La fecha o la hora no son válidas.', 'التاريخ أو الوقت غير صالح.'],
    ['RTC bulunamadı; saat ve tarih ayarlanamadı.', 'RTC not found; time and date could not be set.', 'RTC nicht gefunden; Uhrzeit und Datum konnten nicht eingestellt werden.', 'No se encontró el RTC; no se pudieron ajustar la hora y la fecha.', 'لم يتم العثور على RTC؛ تعذر ضبط الوقت والتاريخ.'],
    ["Saat ve tarih RTC'ye yazılamadı.", 'Time and date could not be written to the RTC.', 'Uhrzeit und Datum konnten nicht in die RTC geschrieben werden.', 'No se pudieron escribir la hora y la fecha en el RTC.', 'تعذرت كتابة الوقت والتاريخ في RTC.'],
    ['Saat ve tarih kaydedildi.', 'Time and date saved.', 'Uhrzeit und Datum gespeichert.', 'Hora y fecha guardadas.', 'تم حفظ الوقت والتاريخ.'],
    ['Uyku ayarı geçersiz.', 'Sleep setting is invalid.', 'Die Ruhezustand-Einstellung ist ungültig.', 'El ajuste de reposo no es válido.', 'إعداد السكون غير صالح.'],
    ['Uyku ayarları flash belleğe kaydedilemedi.', 'Sleep settings could not be saved to flash.', 'Die Ruhezustand-Einstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes de reposo en la memoria flash.', 'تعذر حفظ إعدادات السكون في ذاكرة الفلاش.'],
    ['Panel uyku ayarları kaydedildi.', 'Panel sleep settings saved.', 'Panel-Ruhezustand gespeichert.', 'Ajustes de reposo del panel guardados.', 'تم حفظ إعدادات سكون اللوحة.'],
    ['Eksik veya geçersiz NTP ayarı.', 'Missing or invalid NTP setting.', 'Fehlende oder ungültige NTP-Einstellung.', 'Ajuste NTP ausente o no válido.', 'إعداد NTP مفقود أو غير صالح.'],
    ['NTP ayarları flash belleğe kaydedilemedi.', 'NTP settings could not be saved to flash.', 'Die NTP-Einstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes NTP en la memoria flash.', 'تعذر حفظ إعدادات NTP في ذاكرة الفلاش.'],
    ['NTP ayarları kaydedildi.', 'NTP settings saved.', 'NTP-Einstellungen gespeichert.', 'Ajustes NTP guardados.', 'تم حفظ إعدادات NTP.'],
    ['Alarm sayısı geçersiz.', 'Number of alarms is invalid.', 'Die Anzahl der Alarme ist ungültig.', 'El número de alarmas no es válido.', 'عدد المنبهات غير صالح.'],
    ['Eksik veya geçersiz alarm ayarı.', 'Missing or invalid alarm setting.', 'Fehlende oder ungültige Alarm-Einstellung.', 'Ajuste de alarma ausente o no válido.', 'إعداد المنبه مفقود أو غير صالح.'],
    ['Alarm ayarları flash belleğe kaydedilemedi.', 'Alarm settings could not be saved to flash.', 'Die Alarm-Einstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes de alarmas en la memoria flash.', 'تعذر حفظ إعدادات المنبهات في ذاكرة الفلاش.'],
    ['Alarm ayarları kaydedildi.', 'Alarm settings saved.', 'Alarm-Einstellungen gespeichert.', 'Ajustes de alarmas guardados.', 'تم حفظ إعدادات المنبهات.'],
    ['Eksik Wi-Fi ayarı.', 'Missing Wi-Fi setting.', 'Fehlende Wi-Fi-Einstellung.', 'Falta un ajuste de Wi-Fi.', 'إعداد Wi-Fi مفقود.'],
    ['Wi-Fi ayarları flash belleğe kaydedilemedi.', 'Wi-Fi settings could not be saved to flash.', 'Die Wi-Fi-Einstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes de Wi-Fi en la memoria flash.', 'تعذر حفظ إعدادات Wi-Fi في ذاكرة الفلاش.'],
    ['Wi-Fi ayarları kaydedildi. Cihaz yeniden başlatılacak.', 'Wi-Fi settings saved. The device will restart.', 'Wi-Fi-Einstellungen gespeichert. Das Gerät wird neu gestartet.', 'Ajustes de Wi-Fi guardados. El dispositivo se reiniciará.', 'تم حفظ إعدادات Wi-Fi. سيعاد تشغيل الجهاز.'],
    ['Eksik veya geçersiz haberleşme ayarı.', 'Missing or invalid communication setting.', 'Fehlende oder ungültige Kommunikationseinstellung.', 'Ajuste de comunicación ausente o no válido.', 'إعداد الاتصال مفقود أو غير صالح.'],
    ['Haberleşme ayarları flash belleğe kaydedilemedi.', 'Communication settings could not be saved to flash.', 'Die Kommunikationseinstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes de comunicación en la memoria flash.', 'تعذر حفظ إعدادات الاتصال في ذاكرة الفلاش.'],
    ['Haberleşme ayarları kaydedildi. Cihaz yeniden başlatılacak.', 'Communication settings saved. The device will restart.', 'Kommunikationseinstellungen gespeichert. Das Gerät wird neu gestartet.', 'Ajustes de comunicación guardados. El dispositivo se reiniciará.', 'تم حفظ إعدادات الاتصال. سيعاد تشغيل الجهاز.'],
    ['Test rengi eksik.', 'Test color is missing.', 'Die Testfarbe fehlt.', 'Falta el color de prueba.', 'لون الاختبار مفقود.'],
    ['Panel kullanılabilir değil.', 'The panel is not available.', 'Das Panel ist nicht verfügbar.', 'El panel no está disponible.', 'اللوحة غير متاحة.'],
    ['Geçersiz test rengi.', 'Invalid test color.', 'Ungültige Testfarbe.', 'Color de prueba no válido.', 'لون اختبار غير صالح.'],
    ['Renk testi başlatıldı.', 'Color test started.', 'Farbtest gestartet.', 'Prueba de color iniciada.', 'بدأ اختبار اللون.'],
    ['Yazılım dosyası alınamadı.', 'The firmware file could not be received.', 'Die Firmwaredatei konnte nicht empfangen werden.', 'No se pudo recibir el archivo de firmware.', 'تعذر استلام ملف البرنامج.'],
    ['Dosya adı firmware.bin olmalıdır.', 'The file name must be firmware.bin.', 'Der Dateiname muss firmware.bin lauten.', 'El nombre del archivo debe ser firmware.bin.', 'يجب أن يكون اسم الملف firmware.bin.'],
    ['OTA başlatılamadı', 'OTA could not be started', 'OTA konnte nicht gestartet werden', 'No se pudo iniciar la OTA', 'تعذر بدء التحديث عبر الشبكة'],
    ['Firmware yükleniyor.', 'Uploading firmware.', 'Firmware wird hochgeladen.', 'Cargando el firmware.', 'جارٍ رفع البرنامج.'],
    ['Firmware yazılamadı', 'Firmware could not be written', 'Firmware konnte nicht geschrieben werden', 'No se pudo escribir el firmware', 'تعذرت كتابة البرنامج'],
    ['Firmware dosyası boş.', 'The firmware file is empty.', 'Die Firmwaredatei ist leer.', 'El archivo de firmware está vacío.', 'ملف البرنامج فارغ.'],
    ['Firmware doğrulanamadı', 'Firmware could not be verified', 'Firmware konnte nicht verifiziert werden', 'No se pudo verificar el firmware', 'تعذر التحقق من البرنامج'],
    ['Firmware yüklemesi bağlantı kesildiği için iptal edildi.', 'The firmware upload was canceled because the connection was lost.', 'Der Firmware-Upload wurde abgebrochen, weil die Verbindung getrennt wurde.', 'La carga del firmware se canceló porque se perdió la conexión.', 'أُلغي رفع البرنامج بسبب انقطاع الاتصال.'],
    ['Seçili sensör (TMP112) nem ölçmez; nem gösteren kutuları kaldırın veya veri türünü değiştirin.', 'The selected sensor (TMP112) does not measure humidity; remove the boxes that show humidity or change their data type.', 'Der gewählte Sensor (TMP112) misst keine Feuchte; entfernen Sie die Boxen mit Feuchteanzeige oder ändern Sie ihren Datentyp.', 'El sensor seleccionado (TMP112) no mide la humedad; elimine los cuadros que muestran humedad o cambie su tipo de datos.', 'المستشعر المحدد (TMP112) لا يقيس الرطوبة؛ احذف المربعات التي تعرض الرطوبة أو غيّر نوع بياناتها.'],
    ['Eksik veya geçersiz sensör ayarı.', 'Missing or invalid sensor setting.', 'Fehlende oder ungültige Sensoreinstellung.', 'Ajuste de sensor ausente o no válido.', 'إعداد المستشعر مفقود أو غير صالح.'],
    ['Ekran tasarımında nem gösteren kutular var. TMP112 nem ölçmediği için önce bu kutuları kaldırın veya veri türünü değiştirin.', 'The display design contains boxes that show humidity. The TMP112 does not measure humidity, so remove these boxes or change their data type first.', 'Das Anzeigedesign enthält Boxen mit Feuchteanzeige. Der TMP112 misst keine Feuchte; entfernen Sie diese Boxen zuerst oder ändern Sie ihren Datentyp.', 'El diseño de pantalla tiene cuadros que muestran humedad. El TMP112 no mide la humedad, así que primero elimine esos cuadros o cambie su tipo de datos.', 'يحتوي تصميم الشاشة على مربعات تعرض الرطوبة. لا يقيس TMP112 الرطوبة، لذا احذف هذه المربعات أو غيّر نوع بياناتها أولًا.'],
    ['Sensör ayarları flash belleğe kaydedilemedi.', 'Sensor settings could not be saved to flash.', 'Die Sensoreinstellungen konnten nicht im Flash gespeichert werden.', 'No se pudieron guardar los ajustes del sensor en la memoria flash.', 'تعذر حفظ إعدادات المستشعر في ذاكرة الفلاش.'],
    ['Sıcaklık sensörü ayarları kaydedildi ve uygulandı.', 'Temperature sensor settings saved and applied.', 'Temperatursensor-Einstellungen gespeichert und übernommen.', 'Ajustes del sensor de temperatura guardados y aplicados.', 'تم حفظ إعدادات مستشعر درجة الحرارة وتطبيقها.'],
    ['Sıcaklık sensörü modeli geçersiz.', 'Temperature sensor model is invalid.', 'Das Temperatursensormodell ist ungültig.', 'El modelo de sensor de temperatura no es válido.', 'طراز مستشعر درجة الحرارة غير صالح.'],
    ['Ölçüm aralığı 1-60 saniye olmalıdır.', 'Measurement interval must be 1-60 seconds.', 'Das Messintervall muss 1-60 Sekunden betragen.', 'El intervalo de medición debe ser de 1 a 60 segundos.', 'يجب أن تكون فترة القياس من 1 إلى 60 ثانية.'],
    ['Sıcaklık düzeltmesi -10.0 ile 10.0 °C arasında olmalıdır.', 'Temperature offset must be between -10.0 and 10.0 °C.', 'Die Temperaturkorrektur muss zwischen -10.0 und 10.0 °C liegen.', 'La corrección de temperatura debe estar entre -10.0 y 10.0 °C.', 'يجب أن يكون تصحيح درجة الحرارة بين -10.0 و10.0 °C.'],
    ['Nem düzeltmesi -20.0 ile 20.0 %RH arasında olmalıdır.', 'Humidity offset must be between -20.0 and 20.0 %RH.', 'Die Feuchtekorrektur muss zwischen -20.0 und 20.0 %RH liegen.', 'La corrección de humedad debe estar entre -20.0 y 20.0 %RH.', 'يجب أن يكون تصحيح الرطوبة بين -20.0 و20.0 %RH.'],
    ['SHT40 ayarları geçersiz.', 'SHT40 settings are invalid.', 'Die SHT40-Einstellungen sind ungültig.', 'Los ajustes de SHT40 no son válidos.', 'إعدادات SHT40 غير صالحة.'],
    ['TMP112 ayarları geçersiz.', 'TMP112 settings are invalid.', 'Die TMP112-Einstellungen sind ungültig.', 'Los ajustes de TMP112 no son válidos.', 'إعدادات TMP112 غير صالحة.'],
    ['HDC2022 ayarları geçersiz.', 'HDC2022 settings are invalid.', 'Die HDC2022-Einstellungen sind ungültig.', 'Los ajustes de HDC2022 no son válidos.', 'إعدادات HDC2022 غير صالحة.'],
    ['Geri sayım alanlarından biri geçersiz.', 'One of the countdown fields is invalid.', 'Eines der Countdown-Felder ist ungültig.', 'Uno de los campos de la cuenta atrás no es válido.', 'أحد حقول العد التنازلي غير صالح.'],
    ["Tasarımda geri sayım kutusu yok. Önce bir TextBox'ın veri türünü Geri Sayım yapıp panele uygulayın.", "The design has no countdown box. First set a TextBox's data type to Countdown and apply it to the panel.", 'Das Design enthält keine Countdown-Box. Stellen Sie zuerst den Datentyp einer TextBox auf Countdown und wenden Sie es auf das Panel an.', 'El diseño no tiene ninguna caja de cuenta atrás. Primero establezca el tipo de datos de un TextBox en Cuenta atrás y aplíquelo al panel.', 'لا يحتوي التصميم على مربع عد تنازلي. اضبط أولاً نوع بيانات أحد مربعات TextBox على العد التنازلي ثم طبّقه على اللوحة.'],
    ['Geri sayım başlatıldı.', 'Countdown started.', 'Countdown gestartet.', 'Cuenta atrás iniciada.', 'بدأ العد التنازلي.'],
    ['Geri sayım durduruldu.', 'Countdown stopped.', 'Countdown angehalten.', 'Cuenta atrás detenida.', 'تم إيقاف العد التنازلي.'],
    ['Geri sayım sıfırlandı.', 'Countdown reset.', 'Countdown zurückgesetzt.', 'Cuenta atrás restablecida.', 'تمت إعادة ضبط العد التنازلي.'],
    ['Geçersiz geri sayım komutu.', 'Invalid countdown command.', 'Ungültiger Countdown-Befehl.', 'Comando de cuenta atrás no válido.', 'أمر عد تنازلي غير صالح.'],
    ['Geri sayım içerik döngüsüne eklenemez.', 'A countdown cannot be added to the content cycle.', 'Ein Countdown kann nicht in den Inhaltszyklus aufgenommen werden.', 'Una cuenta atrás no se puede añadir al ciclo de contenido.', 'لا يمكن إضافة العد التنازلي إلى دورة المحتوى.'],
    ['Geri sayım süresi 1 saniye ile 23:59:59 arasında olmalıdır.', 'Countdown duration must be between 1 second and 23:59:59.', 'Die Countdown-Dauer muss zwischen 1 Sekunde und 23:59:59 liegen.', 'La duración de la cuenta atrás debe estar entre 1 segundo y 23:59:59.', 'يجب أن تكون مدة العد التنازلي بين ثانية واحدة و23:59:59.'],
    ['Geri sayım alarm süresi 0-3600 saniye olmalıdır.', 'Countdown alarm time must be 0-3600 seconds.', 'Die Countdown-Alarmdauer muss 0-3600 Sekunden betragen.', 'La duración de la alarma de la cuenta atrás debe ser de 0 a 3600 segundos.', 'يجب أن تكون مدة إنذار العد التنازلي من 0 إلى 3600 ثانية.'],
    ['Panele uygulanmış Geri Sayım kutularını başlatın, durdurun veya sıfırlayın', 'Start, stop or reset the countdown boxes applied to the panel', 'Auf das Panel angewendete Countdown-Boxen starten, anhalten oder zurücksetzen', 'Inicie, detenga o restablezca las cajas de cuenta atrás aplicadas al panel', 'ابدأ مربعات العد التنازلي المطبقة على اللوحة أو أوقفها أو أعد ضبطها'],
    ['Hazır', 'Ready', 'Bereit', 'Lista', 'جاهز'],
    ['Sayıyor', 'Running', 'Läuft', 'En marcha', 'قيد العد'],
    ['Durduruldu', 'Stopped', 'Angehalten', 'Detenida', 'متوقف'],
    ['Süre doldu', "Time's up", 'Zeit abgelaufen', 'Tiempo agotado', 'انتهى الوقت'],
    ['Süre doldu, alarm rölesi çalışıyor. Durdur veya Sıfırla alarmı susturur.', "Time's up, the alarm relay is on. Stop or Reset silences the alarm.", 'Zeit abgelaufen, das Alarmrelais ist aktiv. Anhalten oder Zurücksetzen schaltet den Alarm ab.', 'Tiempo agotado, el relé de alarma está activo. Detener o Restablecer silencia la alarma.', 'انتهى الوقت، مرحل الإنذار يعمل. يؤدي الإيقاف أو إعادة الضبط إلى إسكات الإنذار.'],
    ['Başlat', 'Start', 'Starten', 'Iniciar', 'ابدأ'],
    ['Durdur', 'Stop', 'Anhalten', 'Detener', 'إيقاف'],
    ['Sıfırla', 'Reset', 'Zurücksetzen', 'Restablecer', 'إعادة ضبط'],
    ['Devam Et', 'Resume', 'Fortsetzen', 'Reanudar', 'استئناف'],
    ['Yeniden Başlat', 'Restart', 'Neu starten', 'Volver a iniciar', 'إعادة البدء'],
    ['Geri Sayım', 'Countdown', 'Countdown', 'Cuenta atrás', 'عد تنازلي'],
    ['Geri sayım süresi', 'Countdown duration', 'Countdown-Dauer', 'Duración de la cuenta atrás', 'مدة العد التنازلي'],
    ['Saat (0-23)', 'Hours (0-23)', 'Stunden (0-23)', 'Horas (0-23)', 'الساعات (0-23)'],
    ['Dakika (0-59)', 'Minutes (0-59)', 'Minuten (0-59)', 'Minutos (0-59)', 'الدقائق (0-59)'],
    ['Saniye (0-59)', 'Seconds (0-59)', 'Sekunden (0-59)', 'Segundos (0-59)', 'الثواني (0-59)'],
    ['Saat 0 ise panelde yalnızca dakika ve saniye (40:00), saat ve dakika 0 ise yalnızca saniye (45) gösterilir.', 'With 0 hours the panel shows only minutes and seconds (40:00); with 0 hours and 0 minutes only seconds (45).', 'Bei 0 Stunden zeigt das Panel nur Minuten und Sekunden (40:00), bei 0 Stunden und 0 Minuten nur Sekunden (45).', 'Con 0 horas el panel muestra solo minutos y segundos (40:00); con 0 horas y 0 minutos, solo segundos (45).', 'عند 0 ساعات تعرض اللوحة الدقائق والثواني فقط (40:00)، وعند 0 ساعات و0 دقائق الثواني فقط (45).'],
    ['Alarm (röle) süresi — saniye', 'Alarm (relay) time — seconds', 'Alarmdauer (Relais) — Sekunden', 'Duración de la alarma (relé) — segundos', 'مدة الإنذار (المرحل) — بالثواني'],
    ['Süre bittiğinde alarm rölesi bu kadar çalışır; 0 alarmı kapatır. Panele uyguladıktan sonra Başlat, Durdur ve Sıfırla sayfanın üstündeki Geri Sayım kartında görünür.', 'When the time is up the alarm relay runs this long; 0 disables the alarm. After applying to the panel, Start, Stop and Reset appear on the Countdown card at the top of the page.', 'Nach Ablauf der Zeit bleibt das Alarmrelais so lange aktiv; 0 schaltet den Alarm ab. Nach dem Anwenden auf das Panel erscheinen Starten, Anhalten und Zurücksetzen auf der Karte Countdown oben auf der Seite.', 'Al agotarse el tiempo, el relé de alarma funciona durante este tiempo; 0 desactiva la alarma. Tras aplicarlo al panel, Iniciar, Detener y Restablecer aparecen en la tarjeta Cuenta atrás, en la parte superior de la página.', 'عند انتهاء الوقت يعمل مرحل الإنذار لهذه المدة؛ القيمة 0 تعطّل الإنذار. بعد التطبيق على اللوحة تظهر أزرار ابدأ وإيقاف وإعادة ضبط في بطاقة العد التنازلي أعلى الصفحة.'],
    ['Geri sayım süresi değişti.', 'Countdown duration changed.', 'Countdown-Dauer geändert.', 'Duración de la cuenta atrás cambiada.', 'تم تغيير مدة العد التنازلي.'],
    ['Geri sayım sıfırlansın mı? Geçen süre silinir.', 'Reset the countdown? The elapsed time will be cleared.', 'Countdown zurücksetzen? Die verstrichene Zeit wird gelöscht.', '¿Restablecer la cuenta atrás? Se borrará el tiempo transcurrido.', 'هل تريد إعادة ضبط العد التنازلي؟ سيتم مسح الوقت المنقضي.'],
    ['Alarm modu geçersiz.', 'Alarm mode is invalid.', 'Der Alarmmodus ist ungültig.', 'El modo de alarma no es válido.', 'وضع المنبه غير صالح.'],
    ['Alarm ayarları kaydedildi. Okul saati etkin.', 'Alarm settings saved. School bell is active.', 'Alarm-Einstellungen gespeichert. Die Schulglocke ist aktiv.', 'Ajustes de alarmas guardados. El timbre escolar está activo.', 'تم حفظ إعدادات المنبهات. جرس المدرسة مفعّل.'],
    ['Alarm ayarları kaydedildi. Manuel alarm etkin.', 'Alarm settings saved. Manual alarms are active.', 'Alarm-Einstellungen gespeichert. Manuelle Alarme sind aktiv.', 'Ajustes de alarmas guardados. Las alarmas manuales están activas.', 'تم حفظ إعدادات المنبهات. المنبهات اليدوية مفعّلة.'],
    ['Ders sayısı geçersiz.', 'Number of lessons is invalid.', 'Die Anzahl der Stunden ist ungültig.', 'El número de clases no es válido.', 'عدد الحصص غير صالح.'],
    ['Eksik veya geçersiz ders saati.', 'Missing or invalid lesson time.', 'Fehlende oder ungültige Unterrichtszeit.', 'Hora de clase ausente o no válida.', 'وقت الحصة مفقود أو غير صالح.'],
    ['Okul saati gün seçimi geçersiz.', 'School bell day selection is invalid.', 'Die Tagesauswahl der Schulglocke ist ungültig.', 'La selección de días del timbre escolar no es válida.', 'اختيار أيام جرس المدرسة غير صالح.'],
    ['Okul saati için en az bir gün seçilmelidir.', 'Select at least one day for the school bell.', 'Mindestens einen Tag für die Schulglocke auswählen.', 'Seleccione al menos un día para el timbre escolar.', 'اختر يوماً واحداً على الأقل لجرس المدرسة.'],
    ['Zil süresi 1-3600 saniye olmalıdır.', 'Bell duration must be 1-3600 seconds.', 'Die Klingeldauer muss 1-3600 Sekunden betragen.', 'La duración del timbre debe ser de 1 a 3600 segundos.', 'يجب أن تكون مدة الجرس من 1 إلى 3600 ثانية.'],
    ['Öğrenci zili en fazla 10 dakika önce çalabilir.', 'The student bell can ring at most 10 minutes early.', 'Die Schülerklingel kann höchstens 10 Minuten vorher läuten.', 'El timbre de alumnos puede sonar como máximo 10 minutos antes.', 'يمكن أن يرن جرس الطلاب قبل 10 دقائق كحد أقصى.'],
    ['Tatil dönemi sayısı geçersiz.', 'Number of holiday periods is invalid.', 'Die Anzahl der Ferienzeiträume ist ungültig.', 'El número de períodos de vacaciones no es válido.', 'عدد فترات العطل غير صالح.'],
    ['Tatil dönemi tarihi geçersiz.', 'Holiday period date is invalid.', 'Das Datum des Ferienzeitraums ist ungültig.', 'La fecha del período de vacaciones no es válida.', 'تاريخ فترة العطلة غير صالح.'],
    ['Test süresi 1-30 saniye olmalıdır.', 'Test duration must be 1-30 seconds.', 'Die Testdauer muss 1-30 Sekunden betragen.', 'La duración de la prueba debe ser de 1 a 30 segundos.', 'يجب أن تكون مدة الاختبار من 1 إلى 30 ثانية.'],
    ['Zil test için çalıştırıldı.', 'Bell switched on for a test.', 'Klingel zum Test eingeschaltet.', 'Timbre activado para una prueba.', 'تم تشغيل الجرس للاختبار.'],
    ['Okul saati veri formatı geçersiz.', 'School bell data format is invalid.', 'Das Datenformat der Schulglocke ist ungültig.', 'El formato de datos del timbre escolar no es válido.', 'تنسيق بيانات جرس المدرسة غير صالح.'],
    ['Ders saati geçersiz.', 'Lesson time is invalid.', 'Die Unterrichtszeit ist ungültig.', 'La hora de clase no es válida.', 'وقت الحصة غير صالح.'],
    ['Teneffüs zili ders başlangıcından sonra olmalıdır.', 'The break bell must be after the lesson start.', 'Die Pausenklingel muss nach dem Stundenbeginn liegen.', 'El timbre de recreo debe ser posterior al inicio de la clase.', 'يجب أن يكون جرس الاستراحة بعد بداية الحصة.'],
    ['Dersler sırayla girilmelidir; her ders bir önceki dersin teneffüs zilinden sonra başlamalıdır.', 'Enter lessons in order; each lesson must start after the break bell of the previous lesson.', 'Stunden der Reihe nach eingeben; jede Stunde muss nach der Pausenklingel der vorherigen Stunde beginnen.', 'Introduzca las clases en orden; cada clase debe empezar después del timbre de recreo de la anterior.', 'أدخل الحصص بالترتيب؛ يجب أن تبدأ كل حصة بعد جرس استراحة الحصة السابقة.'],
    ['Öğrenci zili bir önceki dersin teneffüs zilinden sonra çalmalıdır; öğrenci zili süresini kısaltın.', 'The student bell must ring after the break bell of the previous lesson; shorten the student bell lead time.', 'Die Schülerklingel muss nach der Pausenklingel der vorherigen Stunde läuten; Vorlaufzeit der Schülerklingel verkürzen.', 'El timbre de alumnos debe sonar después del timbre de recreo de la clase anterior; acorte su antelación.', 'يجب أن يرن جرس الطلاب بعد جرس استراحة الحصة السابقة؛ قلّل مدة تقديم جرس الطلاب.'],
    ['Tatil döneminin bitişi başlangıcından önce olamaz.', 'A holiday period cannot end before it starts.', 'Ein Ferienzeitraum kann nicht vor seinem Beginn enden.', 'Un período de vacaciones no puede terminar antes de empezar.', 'لا يمكن أن تنتهي فترة العطلة قبل بدايتها.'],
    ['Yılbaşı', "New Year's Day", 'Neujahr', 'Año Nuevo', 'رأس السنة الميلادية'],
    ['Ulusal Egemenlik ve Çocuk Bayramı', "National Sovereignty and Children's Day", 'Tag der nationalen Souveränität und des Kindes', 'Día de la Soberanía Nacional y de los Niños', 'عيد السيادة الوطنية والطفل'],
    ['Emek ve Dayanışma Günü', 'Labour and Solidarity Day', 'Tag der Arbeit und Solidarität', 'Día del Trabajo y la Solidaridad', 'يوم العمل والتضامن'],
    ["Atatürk'ü Anma, Gençlik ve Spor Bayramı", 'Commemoration of Atatürk, Youth and Sports Day', 'Gedenktag an Atatürk, Tag der Jugend und des Sports', 'Día de Conmemoración de Atatürk, de la Juventud y el Deporte', 'عيد إحياء ذكرى أتاتورك والشباب والرياضة'],
    ['Demokrasi ve Millî Birlik Günü', 'Democracy and National Unity Day', 'Tag der Demokratie und der nationalen Einheit', 'Día de la Democracia y la Unidad Nacional', 'يوم الديمقراطية والوحدة الوطنية'],
    ['Zafer Bayramı', 'Victory Day', 'Tag des Sieges', 'Día de la Victoria', 'عيد النصر'],
    ['Cumhuriyet Bayramı arifesi', 'Republic Day eve', 'Vorabend des Tages der Republik', 'Víspera del Día de la República', 'عشية عيد الجمهورية'],
    ['Cumhuriyet Bayramı', 'Republic Day', 'Tag der Republik', 'Día de la República', 'عيد الجمهورية'],
    ['Ramazan Bayramı arifesi', 'Eid al-Fitr eve', 'Vorabend des Ramadanfests', 'Víspera del Eid al-Fitr', 'عشية عيد الفطر'],
    ['Ramazan Bayramı', 'Eid al-Fitr', 'Ramadanfest', 'Eid al-Fitr', 'عيد الفطر'],
    ['Kurban Bayramı arifesi', 'Eid al-Adha eve', 'Vorabend des Opferfests', 'Víspera del Eid al-Adha', 'عشية عيد الأضحى'],
    ['Kurban Bayramı', 'Eid al-Adha', 'Opferfest', 'Eid al-Adha', 'عيد الأضحى'],
    ['IO42 alarm rölesi manuel alarmlarla veya okul saati programıyla çalışır. Aynı anda yalnızca biri etkindir; diğerinin ayarları saklanır.', 'The IO42 alarm relay is driven by manual alarms or by the school bell schedule. Only one of them is active at a time; the settings of the other are kept.', 'Das IO42-Alarmrelais wird von manuellen Alarmen oder vom Schulglocken-Plan gesteuert. Es ist immer nur eines aktiv; die Einstellungen des anderen bleiben gespeichert.', 'El relé de alarma IO42 funciona con alarmas manuales o con el horario del timbre escolar. Solo uno está activo a la vez; los ajustes del otro se conservan.', 'يعمل مرحل المنبه IO42 بالمنبهات اليدوية أو بجدول جرس المدرسة. يكون أحدهما فقط مفعّلاً في كل مرة، وتُحفظ إعدادات الآخر.'],
    ['Manuel alarm', 'Manual alarm', 'Manueller Alarm', 'Alarma manual', 'منبه يدوي'],
    ['Belirlediğiniz saatlerde çalan en fazla 10 alarm', 'Up to 10 alarms at times you choose', 'Bis zu 10 Alarme zu selbst gewählten Zeiten', 'Hasta 10 alarmas a las horas que elija', 'حتى 10 منبهات في الأوقات التي تحددها'],
    ['Okul saati', 'School bell', 'Schulglocke', 'Timbre escolar', 'جرس المدرسة'],
    ['Ders ve teneffüs zilleri, resmi tatiller ve tatil dönemleri', 'Lesson and break bells, public holidays and holiday periods', 'Stunden- und Pausenklingeln, Feiertage und Ferienzeiträume', 'Timbres de clase y recreo, festivos oficiales y períodos de vacaciones', 'أجراس الحصص والاستراحات والعطل الرسمية وفترات العطل'],
    ['Alarm modu', 'Alarm mode', 'Alarmmodus', 'Modo de alarma', 'وضع المنبه'],
    ['Okul saati durumu alınıyor', 'Reading school bell status', 'Status der Schulglocke wird abgerufen', 'Obteniendo el estado del timbre escolar', 'جارٍ جلب حالة جرس المدرسة'],
    ['Genel ayarlar', 'General settings', 'Allgemeine Einstellungen', 'Ajustes generales', 'الإعدادات العامة'],
    ['Çalışma günleri', 'School days', 'Schultage', 'Días lectivos', 'أيام الدراسة'],
    ['Zil süresi (saniye)', 'Bell duration (seconds)', 'Klingeldauer (Sekunden)', 'Duración del timbre (segundos)', 'مدة الجرس (ثانية)'],
    ['Okul zilinin çalışacağı günleri seçin.', 'Select the days on which the school bell rings.', 'Tage auswählen, an denen die Schulglocke läutet.', 'Seleccione los días en que suena el timbre escolar.', 'اختر الأيام التي يرن فيها جرس المدرسة.'],
    ['Öğrenci zili', 'Student bell', 'Schülerklingel', 'Timbre de alumnos', 'جرس الطلاب'],
    ['Öğrenci zili, öğrencileri sınıfa çağırmak için her dersten önce ek bir zil çalar; ders başlangıç zili öğretmen zili olarak çalmaya devam eder. Bütün ziller aynı zil süresini kullanır.', 'The student bell rings an extra bell before each lesson to call students to class; the lesson start bell keeps ringing as the teacher bell. All bells use the same bell duration.', 'Die Schülerklingel läutet vor jeder Stunde zusätzlich, um die Schüler in die Klasse zu rufen; die Klingel zum Stundenbeginn läutet weiterhin als Lehrerklingel. Alle Klingeln verwenden dieselbe Klingeldauer.', 'El timbre de alumnos suena antes de cada clase para llamar a los alumnos al aula; el timbre de inicio de clase sigue sonando como timbre de profesores. Todos los timbres usan la misma duración.', 'يرن جرس الطلاب جرساً إضافياً قبل كل حصة لاستدعاء الطلاب إلى الصف، ويستمر جرس بداية الحصة جرساً للمعلمين. تستخدم كل الأجراس مدة الجرس نفسها.'],
    ['Resmi tatillerde zil çalmasın', 'Do not ring on public holidays', 'An Feiertagen nicht läuten', 'No sonar en festivos oficiales', 'لا يرن في العطل الرسمية'],
    ["Yalnızca Türkiye'deki resmi tatiller", 'Public holidays in Türkiye only', 'Nur gesetzliche Feiertage in der Türkei', 'Solo festivos oficiales de Turquía', 'العطل الرسمية في تركيا فقط'],
    ["Ulusal bayramlarda, 1 Ocak, 1 Mayıs ve 15 Temmuz'da, Ramazan ve Kurban bayramlarında zil çalmaz. Bayram arifelerinde ve 28 Ekim'de saat 13:00'ten sonra zil çalmaz.", 'The bell does not ring on national holidays, on 1 January, 1 May and 15 July, or during Eid al-Fitr and Eid al-Adha. On the eves of these feasts and on 28 October it does not ring after 13:00.', 'An Nationalfeiertagen, am 1. Januar, 1. Mai und 15. Juli sowie am Ramadan- und Opferfest läutet die Klingel nicht. An den Vorabenden der Feste und am 28. Oktober läutet sie nach 13:00 Uhr nicht.', 'El timbre no suena en las fiestas nacionales, el 1 de enero, el 1 de mayo, el 15 de julio ni durante el Eid al-Fitr y el Eid al-Adha. En las vísperas de estas fiestas y el 28 de octubre no suena después de las 13:00.', 'لا يرن الجرس في الأعياد الوطنية وفي 1 يناير و1 مايو و15 يوليو وفي عيدي الفطر والأضحى. ولا يرن بعد الساعة 13:00 في عشية العيدين وفي 28 أكتوبر.'],
    ["10 Kasım'da saat 09:05'te saygı duruşu zili çal", 'Ring for the moment of silence on 10 November at 09:05', 'Am 10. November um 09:05 Uhr zur Schweigeminute läuten', 'Sonar el 10 de noviembre a las 09:05 por el minuto de silencio', 'رنّ جرس الوقوف دقيقة صمت في 10 نوفمبر الساعة 09:05'],
    ['Ders programı', 'Lesson schedule', 'Stundenplan', 'Horario de clases', 'جدول الحصص'],
    ['Dersleri saat sırasıyla ekleyin. Her dersin altına teneffüs zilini ekleyebilirsiniz; son dersin teneffüs zili okul çıkış zili olarak kullanılabilir. İkili öğretimde sabahçı ve öğlenci derslerini aynı listeye sırayla ekleyin.', 'Add lessons in order of time. You can add a break bell under each lesson; the break bell of the last lesson can serve as the end-of-school bell. For double-shift schools add the morning and afternoon lessons to the same list in order.', 'Stunden in zeitlicher Reihenfolge hinzufügen. Unter jeder Stunde kann eine Pausenklingel hinzugefügt werden; die Pausenklingel der letzten Stunde kann als Schulschlussklingel dienen. Bei Schichtunterricht Vormittags- und Nachmittagsstunden der Reihe nach in dieselbe Liste eintragen.', 'Añada las clases por orden de hora. Puede añadir un timbre de recreo bajo cada clase; el de la última clase puede servir de timbre de salida. En centros con doble turno, añada las clases de mañana y tarde en orden en la misma lista.', 'أضف الحصص بترتيب أوقاتها. يمكنك إضافة جرس استراحة أسفل كل حصة، ويمكن استخدام جرس استراحة الحصة الأخيرة جرساً لانتهاء الدوام. في المدارس ذات الفترتين أضف حصص الصباح والمساء إلى القائمة نفسها بالترتيب.'],
    ['0 / 20 ders', '0 / 20 lessons', '0 / 20 Stunden', '0 / 20 clases', '0 / 20 حصة'],
    ['Yeni ders, önceki teneffüsten 10 dakika sonra önerilir', 'A new lesson is suggested 10 minutes after the previous break bell', 'Eine neue Stunde wird 10 Minuten nach der vorherigen Pausenklingel vorgeschlagen', 'Una clase nueva se propone 10 minutos después del timbre de recreo anterior', 'تُقترح الحصة الجديدة بعد 10 دقائق من جرس الاستراحة السابق'],
    ['Henüz ders eklenmedi.', 'No lessons added yet.', 'Noch keine Stunden hinzugefügt.', 'Aún no se han añadido clases.', 'لم تتم إضافة حصص بعد.'],
    ['Ders Ekle', 'Add Lesson', 'Stunde hinzufügen', 'Añadir clase', 'إضافة حصة'],
    ['Tatil dönemleri', 'Holiday periods', 'Ferienzeiträume', 'Períodos de vacaciones', 'فترات العطل'],
    ['Yaz tatili, yarıyıl tatili, ara tatiller veya idari izin gibi okulun kapalı olduğu günleri ekleyin; bu günlerde zil çalmaz. Başlangıç ve bitiş günleri dahildir.', 'Add the days on which the school is closed, such as summer holidays, the semester break, mid-term breaks or administrative leave; the bell does not ring on these days. Start and end days are included.', 'Tage hinzufügen, an denen die Schule geschlossen ist, z. B. Sommerferien, Halbjahresferien, Zwischenferien oder Verwaltungsurlaub; an diesen Tagen läutet die Klingel nicht. Anfangs- und Endtag sind eingeschlossen.', 'Añada los días en que el centro está cerrado, como las vacaciones de verano, de semestre, los descansos intermedios o los permisos administrativos; esos días el timbre no suena. Los días de inicio y fin están incluidos.', 'أضف الأيام التي تكون فيها المدرسة مغلقة، مثل العطلة الصيفية وعطلة نصف السنة والعطل الفصلية أو الإجازات الإدارية؛ لا يرن الجرس في هذه الأيام. يومَا البداية والنهاية مشمولان.'],
    ['0 / 10 tatil dönemi', '0 / 10 holiday periods', '0 / 10 Ferienzeiträume', '0 / 10 períodos de vacaciones', '0 / 10 فترة عطلة'],
    ['Henüz tatil dönemi eklenmedi.', 'No holiday periods added yet.', 'Noch keine Ferienzeiträume hinzugefügt.', 'Aún no se han añadido períodos de vacaciones.', 'لم تتم إضافة فترات عطل بعد.'],
    ['Tatil Dönemi Ekle', 'Add Holiday Period', 'Ferienzeitraum hinzufügen', 'Añadir período de vacaciones', 'إضافة فترة عطلة'],
    ['Zili Test Et', 'Test Bell', 'Klingel testen', 'Probar timbre', 'اختبار الجرس'],
    ['Dersten {minutes} dakika önce', '{minutes} minutes before the lesson', '{minutes} Minuten vor der Stunde', '{minutes} minutos antes de la clase', 'قبل الحصة بـ {minutes} دقيقة'],
    ['Kapalı', 'Off', 'Aus', 'Desactivado', 'متوقف'],
    ['{number}. Ders', 'Lesson {number}', '{number}. Stunde', 'Clase {number}', 'الحصة {number}'],
    ['Ders {minutes} dk', 'Lesson {minutes} min', 'Stunde {minutes} Min.', 'Clase {minutes} min', 'الحصة {minutes} دقيقة'],
    ['Bu teneffüs zili okul çıkış zili olarak çalar.', 'This break bell rings as the end-of-school bell.', 'Diese Pausenklingel läutet als Schulschlussklingel.', 'Este timbre de recreo suena como timbre de salida.', 'يرن جرس الاستراحة هذا جرساً لانتهاء الدوام.'],
    ['Teneffüs {minutes} dk', 'Break {minutes} min', 'Pause {minutes} Min.', 'Recreo {minutes} min', 'الاستراحة {minutes} دقيقة'],
    ['Sonraki ders teneffüs zilinden sonra başlamalıdır.', 'The next lesson must start after the break bell.', 'Die nächste Stunde muss nach der Pausenklingel beginnen.', 'La siguiente clase debe empezar después del timbre de recreo.', 'يجب أن تبدأ الحصة التالية بعد جرس الاستراحة.'],
    ['Sonraki ders bu dersten sonra başlamalıdır.', 'The next lesson must start after this lesson.', 'Die nächste Stunde muss nach dieser Stunde beginnen.', 'La siguiente clase debe empezar después de esta.', 'يجب أن تبدأ الحصة التالية بعد هذه الحصة.'],
    ['{count} / {maximum} ders', '{count} / {maximum} lessons', '{count} / {maximum} Stunden', '{count} / {maximum} clases', '{count} / {maximum} حصة'],
    ['En fazla {maximum} ders eklenebilir.', 'Up to {maximum} lessons can be added.', 'Es können höchstens {maximum} Stunden hinzugefügt werden.', 'Se pueden añadir como máximo {maximum} clases.', 'يمكن إضافة {maximum} حصة كحد أقصى.'],
    ['Tatil dönemi {number}', 'Holiday period {number}', 'Ferienzeitraum {number}', 'Período de vacaciones {number}', 'فترة العطلة {number}'],
    ['{count} / {maximum} tatil dönemi', '{count} / {maximum} holiday periods', '{count} / {maximum} Ferienzeiträume', '{count} / {maximum} períodos de vacaciones', '{count} / {maximum} فترة عطلة'],
    ['En fazla {maximum} tatil dönemi eklenebilir.', 'Up to {maximum} holiday periods can be added.', 'Es können höchstens {maximum} Ferienzeiträume hinzugefügt werden.', 'Se pueden añadir como máximo {maximum} períodos de vacaciones.', 'يمكن إضافة {maximum} فترة عطلة كحد أقصى.'],
    ['10 Kasım saygı duruşu zili', '10 November moment of silence bell', 'Klingel zur Schweigeminute am 10. November', 'Timbre del minuto de silencio del 10 de noviembre', 'جرس الوقوف دقيقة صمت في 10 نوفمبر'],
    ['{number}. ders teneffüs zili', 'Lesson {number} break bell', 'Pausenklingel der {number}. Stunde', 'Timbre de recreo de la clase {number}', 'جرس استراحة الحصة {number}'],
    ['{number}. ders öğrenci zili', 'Lesson {number} student bell', 'Schülerklingel der {number}. Stunde', 'Timbre de alumnos de la clase {number}', 'جرس الطلاب للحصة {number}'],
    ['{number}. ders zili', 'Lesson {number} bell', 'Klingel der {number}. Stunde', 'Timbre de la clase {number}', 'جرس الحصة {number}'],
    ['Yaklaşan resmi tatiller', 'Upcoming public holidays', 'Kommende Feiertage', 'Próximos festivos oficiales', 'العطل الرسمية القادمة'],
    ['13:00 sonrası', 'after 13:00', 'ab 13:00 Uhr', 'después de las 13:00', 'بعد الساعة 13:00'],
    ['Dini bayram tarihleri Diyanet takvimine göre {year} yılına kadar tanımlıdır.', 'Religious holiday dates are built in from the Diyanet calendar up to {year}.', 'Die Daten der religiösen Feste sind laut Diyanet-Kalender bis {year} hinterlegt.', 'Las fechas de las fiestas religiosas están definidas hasta {year} según el calendario de Diyanet.', 'تواريخ الأعياد الدينية معرّفة حتى عام {year} وفق تقويم رئاسة الشؤون الدينية.'],
    ['Okul saati etkin değil', 'School bell is not active', 'Schulglocke ist nicht aktiv', 'El timbre escolar no está activo', 'جرس المدرسة غير مفعّل'],
    ['Okul saati seçilip kaydedildiğinde zil programı çalışmaya başlar.', 'The bell schedule starts once School bell is selected and saved.', 'Der Klingelplan startet, sobald Schulglocke ausgewählt und gespeichert ist.', 'El horario de timbres empieza al seleccionar y guardar Timbre escolar.', 'يبدأ جدول الأجراس بالعمل عند اختيار جرس المدرسة وحفظه.'],
    ['Saat okunamıyor', 'Clock cannot be read', 'Uhr kann nicht gelesen werden', 'No se puede leer el reloj', 'تعذرت قراءة الساعة'],
    ['DS3231 saati bulunamadığı için okul zili çalmaz.', 'The school bell does not ring because the DS3231 clock was not found.', 'Die Schulglocke läutet nicht, weil die DS3231-Uhr nicht gefunden wurde.', 'El timbre escolar no suena porque no se encontró el reloj DS3231.', 'لا يرن جرس المدرسة لعدم العثور على ساعة DS3231.'],
    ['Bugün okul günü', 'Today is a school day', 'Heute ist ein Schultag', 'Hoy es día lectivo', 'اليوم يوم دراسي'],
    ['Bugün yarım gün resmi tatil', 'Today is a half-day public holiday', 'Heute ist ein halber Feiertag', 'Hoy es festivo oficial de medio día', 'اليوم عطلة رسمية لنصف يوم'],
    ['{holiday}: saat 13:00 sonrasında zil çalmaz.', '{holiday}: the bell does not ring after 13:00.', '{holiday}: nach 13:00 Uhr läutet die Klingel nicht.', '{holiday}: el timbre no suena después de las 13:00.', '{holiday}: لا يرن الجرس بعد الساعة 13:00.'],
    ['Bugün resmi tatil', 'Today is a public holiday', 'Heute ist ein Feiertag', 'Hoy es festivo oficial', 'اليوم عطلة رسمية'],
    ['Bugün tatil dönemi', 'Today is in a holiday period', 'Heute ist Ferienzeit', 'Hoy es período de vacaciones', 'اليوم ضمن فترة عطلة'],
    ['Bugün okul günü değil', 'Today is not a school day', 'Heute ist kein Schultag', 'Hoy no es día lectivo', 'اليوم ليس يوماً دراسياً'],
    ['bugün', 'today', 'heute', 'hoy', 'اليوم'],
    ['yarın', 'tomorrow', 'morgen', 'mañana', 'غداً'],
    ['Sıradaki zil: {day} {time} — {bell}', 'Next bell: {day} {time} — {bell}', 'Nächste Klingel: {day} {time} — {bell}', 'Próximo timbre: {day} {time} — {bell}', 'الجرس التالي: {day} {time} — {bell}'],
    ['Programda çalacak zil yok.', 'No bell is scheduled.', 'Es ist keine Klingel geplant.', 'No hay ningún timbre programado.', 'لا يوجد جرس مجدول.'],
    ['Okul saati için en az bir ders ekleyin.', 'Add at least one lesson for the school bell.', 'Mindestens eine Stunde für die Schulglocke hinzufügen.', 'Añada al menos una clase para el timbre escolar.', 'أضف حصة واحدة على الأقل لجرس المدرسة.'],
    ['{number}. dersin saati eksik.', 'The time of lesson {number} is missing.', 'Die Zeit der {number}. Stunde fehlt.', 'Falta la hora de la clase {number}.', 'وقت الحصة {number} مفقود.'],
    ['{number}. dersin teneffüs zili ders başlangıcından sonra olmalıdır.', 'The break bell of lesson {number} must be after the lesson start.', 'Die Pausenklingel der {number}. Stunde muss nach dem Stundenbeginn liegen.', 'El timbre de recreo de la clase {number} debe ser posterior a su inicio.', 'يجب أن يكون جرس استراحة الحصة {number} بعد بدايتها.'],
    ['{number}. ders, bir önceki dersin teneffüs zilinden sonra başlamalıdır.', 'Lesson {number} must start after the break bell of the previous lesson.', 'Die {number}. Stunde muss nach der Pausenklingel der vorherigen Stunde beginnen.', 'La clase {number} debe empezar después del timbre de recreo de la anterior.', 'يجب أن تبدأ الحصة {number} بعد جرس استراحة الحصة السابقة.'],
    ['{number}. dersin öğrenci zili bir önceki dersin teneffüs zilinden sonra çalmalıdır; öğrenci zili süresini kısaltın.', 'The student bell of lesson {number} must ring after the break bell of the previous lesson; shorten the student bell lead time.', 'Die Schülerklingel der {number}. Stunde muss nach der Pausenklingel der vorherigen Stunde läuten; Vorlaufzeit verkürzen.', 'El timbre de alumnos de la clase {number} debe sonar después del timbre de recreo anterior; acorte su antelación.', 'يجب أن يرن جرس الطلاب للحصة {number} بعد جرس استراحة الحصة السابقة؛ قلّل مدة التقديم.'],
    ['Tatil dönemi {number} için başlangıç ve bitiş tarihi girin.', 'Enter a start and end date for holiday period {number}.', 'Anfangs- und Enddatum für Ferienzeitraum {number} eingeben.', 'Introduzca la fecha de inicio y fin del período de vacaciones {number}.', 'أدخل تاريخي البداية والنهاية لفترة العطلة {number}.'],
    ['Tatil dönemi {number}: bitiş tarihi başlangıçtan önce olamaz.', 'Holiday period {number}: the end date cannot be before the start date.', 'Ferienzeitraum {number}: Das Enddatum darf nicht vor dem Anfangsdatum liegen.', 'Período de vacaciones {number}: la fecha de fin no puede ser anterior a la de inicio.', 'فترة العطلة {number}: لا يمكن أن يسبق تاريخ النهاية تاريخ البداية.'],
    ['Ders', 'Lesson', 'Stunde', 'Clase', 'حصة'],
    ['Ders başlangıcı', 'Lesson start', 'Stundenbeginn', 'Inicio de clase', 'بداية الحصة'],
    ['Teneffüs zili (ders bitişi)', 'Break bell (lesson end)', 'Pausenklingel (Stundenende)', 'Timbre de recreo (fin de clase)', 'جرس الاستراحة (نهاية الحصة)'],
    ['+ Teneffüs Ekle', '+ Add Break', '+ Pause hinzufügen', '+ Añadir recreo', '+ إضافة استراحة'],
    ['Teneffüsü Kaldır', 'Remove Break', 'Pause entfernen', 'Quitar recreo', 'إزالة الاستراحة'],
    ['Tatil dönemi', 'Holiday period', 'Ferienzeitraum', 'Período de vacaciones', 'فترة عطلة'],
    ['Başlangıç tarihi', 'Start date', 'Anfangsdatum', 'Fecha de inicio', 'تاريخ البداية'],
    ['Bitiş tarihi', 'End date', 'Enddatum', 'Fecha de fin', 'تاريخ النهاية'],
    ['Başka bir güncelleme işlemi sürüyor.', 'Another update is in progress.', 'Eine andere Aktualisierung läuft bereits.', 'Ya hay otra actualización en curso.', 'هناك عملية تحديث أخرى قيد التنفيذ.'],
    ['STA ağına bağlı değil; güncelleme denetimi için internet bağlantısı gerekiyor.', 'Not connected to the STA network; checking for updates needs an internet connection.', 'Nicht mit dem STA-Netz verbunden; die Updateprüfung benötigt eine Internetverbindung.', 'No está conectado a la red STA; la comprobación de actualizaciones necesita conexión a Internet.', 'غير متصل بشبكة STA؛ يتطلب التحقق من التحديثات اتصالاً بالإنترنت.'],
    ['Güncelleme işlemi başlatılamadı.', 'The update task could not be started.', 'Der Aktualisierungsvorgang konnte nicht gestartet werden.', 'No se pudo iniciar la actualización.', 'تعذر بدء عملية التحديث.'],
    ['Güncelleme denetimi başlatıldı.', 'Update check started.', 'Updateprüfung gestartet.', 'Comprobación de actualizaciones iniciada.', 'بدأ التحقق من التحديثات.'],
    ['Kurulacak yeni sürüm yok. Önce güncellemeleri denetleyin.', 'There is no new version to install. Check for updates first.', 'Keine neue Version zum Installieren. Zuerst nach Updates suchen.', 'No hay ninguna versión nueva para instalar. Compruebe primero las actualizaciones.', 'لا يوجد إصدار جديد للتثبيت. تحقق من التحديثات أولاً.'],
    ['Güncelleme indiriliyor.', 'Downloading the update.', 'Update wird heruntergeladen.', 'Descargando la actualización.', 'جارٍ تنزيل التحديث.'],
    ['Yetersiz bellek; güncelleme sunucusuna bağlanılamadı.', 'Not enough memory; could not connect to the update server.', 'Nicht genügend Speicher; keine Verbindung zum Update-Server möglich.', 'Memoria insuficiente; no se pudo conectar al servidor de actualizaciones.', 'الذاكرة غير كافية؛ تعذر الاتصال بخادم التحديث.'],
    ['Güncelleme bilgisi alınamadı', 'Update information could not be retrieved', 'Update-Informationen konnten nicht abgerufen werden', 'No se pudo obtener la información de actualización', 'تعذر الحصول على معلومات التحديث'],
    ['Güncelleme bilgisi geçersiz.', 'Update information is invalid.', 'Die Update-Informationen sind ungültig.', 'La información de actualización no es válida.', 'معلومات التحديث غير صالحة.'],
    ['Firmware dosyası bu cihazın güncelleme alanına sığmıyor.', 'The firmware file does not fit into the update partition of this device.', 'Die Firmware-Datei passt nicht in den Update-Bereich dieses Geräts.', 'El archivo de firmware no cabe en la partición de actualización de este dispositivo.', 'ملف البرنامج الثابت لا يتسع في قسم التحديث لهذا الجهاز.'],
    ['Firmware indirilemedi', 'Firmware could not be downloaded', 'Firmware konnte nicht heruntergeladen werden', 'No se pudo descargar el firmware', 'تعذر تنزيل البرنامج الثابت'],
    ['İndirilen firmware güncelleme bilgisiyle uyuşmuyor; birkaç dakika sonra tekrar deneyin.', 'The downloaded firmware does not match the update information; try again in a few minutes.', 'Die heruntergeladene Firmware passt nicht zu den Update-Informationen; in einigen Minuten erneut versuchen.', 'El firmware descargado no coincide con la información de actualización; vuelva a intentarlo en unos minutos.', 'البرنامج الثابت الذي تم تنزيله لا يطابق معلومات التحديث؛ أعد المحاولة بعد بضع دقائق.'],
    ['Çevrimiçi güncelleme sürerken dosya yüklenemez.', 'A file cannot be uploaded while the online update is running.', 'Während der Online-Aktualisierung kann keine Datei hochgeladen werden.', 'No se puede subir un archivo mientras se ejecuta la actualización en línea.', 'لا يمكن رفع ملف أثناء تنفيذ التحديث عبر الإنترنت.'],
    ['Otomatik güncelleme denetimi açıldı.', 'Automatic update check turned on.', 'Automatische Updateprüfung eingeschaltet.', 'Comprobación automática de actualizaciones activada.', 'تم تشغيل التحقق التلقائي من التحديثات.'],
    ['Otomatik güncelleme denetimi kapatıldı.', 'Automatic update check turned off.', 'Automatische Updateprüfung ausgeschaltet.', 'Comprobación automática de actualizaciones desactivada.', 'تم إيقاف التحقق التلقائي من التحديثات.'],
    ['Geçersiz güncelleme komutu.', 'Invalid update command.', 'Ungültiger Aktualisierungsbefehl.', 'Comando de actualización no válido.', 'أمر تحديث غير صالح.'],
    ['Yeni', 'New', 'Neu', 'Nuevo', 'جديد'],
    ['Yeni sürümü GitHub üzerinden çevrimiçi kurabilir veya firmware.bin dosyasını bilgisayarınızdan ya da telefonunuzdan yükleyebilirsiniz. Kurulumdan sonra cihaz otomatik olarak yeniden başlar.', 'Install a new version online from GitHub or upload a firmware.bin file from your computer or phone. The device restarts automatically after the installation.', 'Neue Versionen online von GitHub installieren oder eine firmware.bin-Datei vom Computer oder Telefon hochladen. Nach der Installation startet das Gerät automatisch neu.', 'Instale una versión nueva en línea desde GitHub o suba un archivo firmware.bin desde su ordenador o teléfono. El dispositivo se reinicia automáticamente tras la instalación.', 'ثبّت الإصدار الجديد عبر الإنترنت من GitHub أو ارفع ملف firmware.bin من الحاسوب أو الهاتف. يعيد الجهاز التشغيل تلقائياً بعد التثبيت.'],
    ['Çevrimiçi güncelleme', 'Online update', 'Online-Aktualisierung', 'Actualización en línea', 'التحديث عبر الإنترنت'],
    ['Güncelleme durumu alınıyor', 'Reading update status', 'Aktualisierungsstatus wird abgerufen', 'Obteniendo el estado de la actualización', 'جارٍ جلب حالة التحديث'],
    ['Güncellemeleri otomatik denetle (günde bir kez)', 'Check for updates automatically (once a day)', 'Automatisch nach Updates suchen (einmal täglich)', 'Buscar actualizaciones automáticamente (una vez al día)', 'التحقق من التحديثات تلقائياً (مرة يومياً)'],
    ["Cihaz STA ağı üzerinden internete bağlıyken GitHub üzerinde yayınlanan son firmware'i denetler. Yeni sürüm yalnızca siz onayladığınızda indirilip kurulur. Yeni firmware ilk 60 saniye içinde kapanır veya yeniden başlarsa cihaz önceki sürüme döner.", 'While connected to the internet through the STA network, the device checks the latest firmware published on GitHub. A new version is downloaded and installed only after you confirm it. If the new firmware stops or restarts within its first 60 seconds, the device returns to the previous version.', 'Ist das Gerät über das STA-Netz mit dem Internet verbunden, prüft es die neueste auf GitHub veröffentlichte Firmware. Eine neue Version wird erst nach Ihrer Bestätigung heruntergeladen und installiert. Stoppt die neue Firmware oder startet sie in den ersten 60 Sekunden neu, kehrt das Gerät zur vorherigen Version zurück.', 'Con conexión a Internet a través de la red STA, el dispositivo comprueba el firmware más reciente publicado en GitHub. Una versión nueva solo se descarga e instala tras su confirmación. Si el nuevo firmware se detiene o reinicia en sus primeros 60 segundos, el dispositivo vuelve a la versión anterior.', 'عندما يكون الجهاز متصلاً بالإنترنت عبر شبكة STA يتحقق من أحدث برنامج ثابت منشور على GitHub. لا يتم تنزيل الإصدار الجديد وتثبيته إلا بعد تأكيدك. إذا توقف البرنامج الجديد أو أعاد التشغيل خلال أول 60 ثانية يعود الجهاز إلى الإصدار السابق.'],
    ['Şimdi Denetle', 'Check Now', 'Jetzt prüfen', 'Comprobar ahora', 'تحقق الآن'],
    ['Güncelle', 'Update', 'Aktualisieren', 'Actualizar', 'تحديث'],
    ['Dosyadan güncelleme', 'Update from file', 'Aktualisierung aus Datei', 'Actualización desde archivo', 'التحديث من ملف'],
    ['Çevrimiçi güncelleme ilerlemesi', 'Online update progress', 'Fortschritt der Online-Aktualisierung', 'Progreso de la actualización en línea', 'تقدم التحديث عبر الإنترنت'],
    ['az önce', 'just now', 'gerade eben', 'hace un momento', 'قبل لحظات'],
    ['{minutes} dakika önce', '{minutes} minutes ago', 'vor {minutes} Minuten', 'hace {minutes} minutos', 'قبل {minutes} دقيقة'],
    ['{hours} saat önce', '{hours} hours ago', 'vor {hours} Stunden', 'hace {hours} horas', 'قبل {hours} ساعة'],
    ['Güncellemeler denetleniyor', 'Checking for updates', 'Suche nach Updates', 'Buscando actualizaciones', 'جارٍ التحقق من التحديثات'],
    ['GitHub sunucusuna bağlanılıyor…', 'Connecting to the GitHub server…', 'Verbindung zum GitHub-Server…', 'Conectando con el servidor de GitHub…', 'جارٍ الاتصال بخادم GitHub…'],
    ['Güncelleme indiriliyor', 'Downloading update', 'Update wird heruntergeladen', 'Descargando actualización', 'جارٍ تنزيل التحديث'],
    ['{version} sürümü indirilip kuruluyor. Bu sırada cihazın elektriğini kesmeyin.', 'Version {version} is being downloaded and installed. Do not switch off the device meanwhile.', 'Version {version} wird heruntergeladen und installiert. Das Gerät währenddessen nicht ausschalten.', 'Se está descargando e instalando la versión {version}. No apague el dispositivo mientras tanto.', 'جارٍ تنزيل الإصدار {version} وتثبيته. لا تفصل الكهرباء عن الجهاز خلال ذلك.'],
    ['Güncelleme kuruldu', 'Update installed', 'Update installiert', 'Actualización instalada', 'تم تثبيت التحديث'],
    ['Cihaz yeniden başlatılıyor; sayfa cihaz açılınca kendiliğinden yenilenir.', 'The device is restarting; the page reloads by itself once the device is back.', 'Das Gerät startet neu; die Seite lädt automatisch neu, sobald es wieder erreichbar ist.', 'El dispositivo se está reiniciando; la página se recarga sola cuando vuelva a estar disponible.', 'يعيد الجهاز التشغيل؛ ستُحدَّث الصفحة تلقائياً عند عودة الجهاز.'],
    ['Güncelleme işlemi başarısız', 'Update failed', 'Aktualisierung fehlgeschlagen', 'La actualización ha fallado', 'فشل التحديث'],
    ['Yeni sürüm bulundu', 'New version found', 'Neue Version gefunden', 'Nueva versión encontrada', 'تم العثور على إصدار جديد'],
    ['{version} sürümü kurulabilir. Yüklü sürüm: {current}.', 'Version {version} can be installed. Installed version: {current}.', 'Version {version} kann installiert werden. Installierte Version: {current}.', 'Se puede instalar la versión {version}. Versión instalada: {current}.', 'يمكن تثبيت الإصدار {version}. الإصدار المثبت: {current}.'],
    ['Yazılım güncel', 'Firmware is up to date', 'Firmware ist aktuell', 'El firmware está actualizado', 'البرنامج الثابت محدّث'],
    ['Yüklü sürüm GitHub üzerindeki en son sürüm.', 'The installed version is the latest one on GitHub.', 'Die installierte Version ist die neueste auf GitHub.', 'La versión instalada es la más reciente en GitHub.', 'الإصدار المثبت هو أحدث إصدار على GitHub.'],
    ['Henüz denetlenmedi', 'Not checked yet', 'Noch nicht geprüft', 'Aún no comprobado', 'لم يتم التحقق بعد'],
    ['Otomatik denetim, STA bağlantısından birkaç dakika sonra yapılır.', 'The automatic check runs a few minutes after the STA connection is up.', 'Die automatische Prüfung erfolgt einige Minuten nach dem Aufbau der STA-Verbindung.', 'La comprobación automática se realiza unos minutos después de conectarse a la red STA.', 'يتم التحقق التلقائي بعد دقائق قليلة من الاتصال بشبكة STA.'],
    ['Güncellemeleri denetlemek için cihazın STA ağı üzerinden internete bağlı olması gerekir.', 'To check for updates the device must be connected to the internet through the STA network.', 'Für die Updateprüfung muss das Gerät über das STA-Netz mit dem Internet verbunden sein.', 'Para buscar actualizaciones, el dispositivo debe estar conectado a Internet a través de la red STA.', 'للتحقق من التحديثات يجب أن يكون الجهاز متصلاً بالإنترنت عبر شبكة STA.'],
    ['Son denetim: {age}.', 'Last check: {age}.', 'Letzte Prüfung: {age}.', 'Última comprobación: {age}.', 'آخر تحقق: {age}.'],
    ['{version} sürümü başlatılamadı; cihaz önceki sürüme döndü.', 'Version {version} did not start; the device returned to the previous version.', 'Version {version} ist nicht gestartet; das Gerät ist zur vorherigen Version zurückgekehrt.', 'La versión {version} no arrancó; el dispositivo volvió a la versión anterior.', 'تعذر تشغيل الإصدار {version}؛ عاد الجهاز إلى الإصدار السابق.'],
    ['Yeni sürüm: {version}', 'New version: {version}', 'Neue Version: {version}', 'Nueva versión: {version}', 'الإصدار الجديد: {version}'],
    ['Son değişiklik: {notes}', 'Latest change: {notes}', 'Letzte Änderung: {notes}', 'Último cambio: {notes}', 'آخر تغيير: {notes}'],
    ['{version} Sürümüne Güncelle', 'Update to {version}', 'Auf {version} aktualisieren', 'Actualizar a {version}', 'التحديث إلى {version}'],
    ['Cihaz {version} sürümüne güncellenecek ve yeniden başlayacak. Güncelleme sırasında cihazın elektriğini kesmeyin. Devam edilsin mi?', 'The device will be updated to version {version} and restart. Do not switch off the device during the update. Continue?', 'Das Gerät wird auf Version {version} aktualisiert und neu gestartet. Während der Aktualisierung nicht ausschalten. Fortfahren?', 'El dispositivo se actualizará a la versión {version} y se reiniciará. No lo apague durante la actualización. ¿Continuar?', 'سيتم تحديث الجهاز إلى الإصدار {version} وإعادة تشغيله. لا تفصل الكهرباء أثناء التحديث. هل تريد المتابعة؟']
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

  // Device messages are Turkish keys of this table. Messages such as
  // "OTA başlatılamadı: <library error>" translate the known prefix. An
  // unknown message is shown as sent rather than hidden behind the fallback.
  function server(message, fallback = 'İşlem başarısız.') {
    const source = String(message || '');
    if (!source) return t(fallback);
    if (language === 'tr' || dictionaries[language][source]) return t(source);
    const separator = source.indexOf(': ');
    if (separator > 0 && dictionaries[language][source.slice(0, separator)]) {
      return t(source.slice(0, separator)) + source.slice(separator);
    }
    return source;
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
