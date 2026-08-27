# VSK

**VSK** لغة برمجية خفيفة ومستقلة مصممة لكتابة السكربتات بطريقة واضحة ومرنة

تعمل VSK من خلال ملف نصي بامتداد `.vk` ويبدأ تشغيل البرنامج من الطرفية باستخدام الأمر `vsk`

```bash
vsk bok.vk
```

## كيف تعمل VSK

يقرأ برنامج VSK الملف البرمجي ثم يحلل الأوامر وينفذها بالترتيب

تدعم اللغة المتغيرات والقيم النصية والأرقام والشروط والحلقات والدوال والمكتبات

أسماء الأوامر والمتغيرات غير حساسة لحالة الأحرف

```vsk
set Name = "Ali"
say name
```

الناتج

```text
Ali
```

## مثال سريع

```vsk
set name = "VSK"
set number = 10
number += 5

say name
say number

when number > 10 {
    say "the number is greater than ten"
} otherwise {
    say "the number is ten or less"
}
```

## الصياغة الأساسية

| الاستخدام | الصياغة |
|---|---|
| إنشاء متغير | `set name = value` |
| تعديل متغير | `name = value` |
| زيادة قيمة | `name += value` |
| طباعة قيمة | `say value` |
| شرط | `when condition { ... }` |
| فرع بديل | `otherwise { ... }` |
| حلقة | `repeat condition { ... }` |
| استيراد وحدة | `bring Math` |
| تعريف دالة | `proc name(args) { ... }` |
| إعادة قيمة | `give value` |
| تعليق | `# comment` أو `// comment` |

لشرح الدوال والوحدات بالتفصيل راجع ملف [FUNCTIONS.md](FUNCTIONS.md)

## تنزيل VSK

الحزمة الجاهزة حاليًا مخصصة لأنظمة Debian وUbuntu على أجهزة `amd64`

يمكن تنزيلها مباشرة من صفحة الإصدار

[تنزيل VSK لنظام Debian وUbuntu](https://github.com/xAboodPBx/vsk-language/releases/download/v0.3.0/vsk_0.3.0_amd64.deb)

## تثبيت VSK

بعد تنزيل الملف افتح الطرفية داخل مجلد التنزيل ثم نفذ

```bash
sudo dpkg -i vsk_0.3.0_amd64.deb
```

إذا طلب النظام تثبيت اعتماديات ناقصة نفذ

```bash
sudo apt install -f
```

## تشغيل أول برنامج

أنشئ ملفًا باسم `bok.vk` واكتب داخله

```vsk
say "Hello from VSK"
```

شغل الملف باستخدام

```bash
vsk bok.vk
```

للتحقق من رقم الإصدار

```bash
vsk --version
```

لرؤية أوامر التشغيل

```bash
vsk --help
```

## أنظمة التشغيل

| النظام | طريقة التثبيت الحالية |
|---|---|
| Ubuntu amd64 | ملف Debian مع `dpkg` |
| Debian amd64 | ملف Debian مع `dpkg` |
| Linux arm64 | البناء من المصدر حاليًا |
| Fedora و RHEL | البناء من المصدر حاليًا |
| Windows و macOS | البناء من المصدر حاليًا |

## روابط المشروع

[مستودع VSK على GitHub](https://github.com/xAboodPBx/vsk-language)

[صفحة إصدارات VSK](https://github.com/xAboodPBx/vsk-language/releases)
