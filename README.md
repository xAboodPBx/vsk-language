# VSK

**VSK** لغة برمجية خفيفة ومستقلة لكتابة السكربتات بطريقة واضحة ومرنة

تعمل VSK من خلال ملفات نصية بامتداد `.vk` وتنفذ الأوامر بالترتيب من خلال الأمر `vsk`

```bash
vsk bok.vk
```

## فكرة اللغة

تسمح VSK بإنشاء المتغيرات وكتابة النصوص والأرقام وبناء الشروط والحلقات والدوال واستعمال الوحدات الجاهزة

لا تفرق اللغة بين الحروف الكبيرة والصغيرة في أسماء الأوامر والمتغيرات

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
| تغيير متغير | `name = value` |
| زيادة قيمة | `name += value` |
| طباعة قيمة | `say value` |
| شرط | `when condition { ... }` |
| فرع بديل | `otherwise { ... }` |
| حلقة | `repeat condition { ... }` |
| استيراد وحدة | `bring Math` |
| تعريف دالة | `proc name(args) { ... }` |
| إعادة قيمة | `give value` |
| تعليق | `# comment` أو `// comment` |

لشرح الدوال والوحدات والأوامر بالتفصيل افتح [دليل الدوال](FUNCTIONS.md)

## تنزيل VSK

الحزمة الجاهزة حاليًا مخصصة لأنظمة Debian وUbuntu على أجهزة `amd64`

[تنزيل VSK لنظام Debian وUbuntu](https://github.com/xAboodPBx/vsk-language/releases/download/v0.3.0/vsk_0.3.0_amd64.deb)

[مشاهدة جميع إصدارات VSK](https://github.com/xAboodPBx/vsk-language/releases)

## تثبيت VSK

بعد تنزيل الحزمة افتح الطرفية داخل مجلد التنزيل ثم نفذ

```bash
sudo dpkg -i vsk_0.3.0_amd64.deb
```

إذا ظهرت رسالة عن اعتماديات ناقصة نفذ

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

لعرض رقم الإصدار

```bash
vsk --version
```

لعرض تعليمات الأمر

```bash
vsk --help
```

## الأنظمة المدعومة حاليًا

| النظام | طريقة التثبيت |
|---|---|
| Ubuntu amd64 | حزمة Debian باستخدام `dpkg` |
| Debian amd64 | حزمة Debian باستخدام `dpkg` |
| Linux arm64 | البناء من المصدر |
| Fedora وRHEL | البناء من المصدر |
| Windows وmacOS | البناء من المصدر |

## المستودع

[مستودع VSK على GitHub](https://github.com/xAboodPBx/vsk-language)
