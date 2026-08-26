# VSK — لغة مستقلة

**VSK** لغة برمجية مستقلة في قواعدها وكلماتها ونموذجها التنفيذي. الإصدار الحالي `0.2` لا يستخدم Python أو Go لتفسير البرامج، ولا ينسخ قواعدهما. نواة التنفيذ موجودة في `vsk.c` وتُبنى إلى برنامج تنفيذي أصلي باسم `vskc`.

> لغة VSK لا تتفرع من Python أو Go. لغة C مستخدمة فقط كـ **bootstrap host** أولي لبناء النواة؛ أما قواعد VSK وآلة تنفيذها فمُعرّفة داخل مشروع VSK نفسه.

## التشغيل

ابنِ النواة:

```bash
make
```

ثم شغّل برنامج VSK:

```bash
./vsk demo_independent.vsk
```

ويمكن كذلك تشغيل الملف التنفيذي مباشرة:

```bash
./vskc demo_independent.vsk
```

## مثال كامل

```vsk
BRING Math
bring unused_package

set Name = "VSK"
set score = 10
SCORE += 5

say name
say Math.sqrt(81)

proc greet(person) {
    say "hello " + person
    give 42
}

set result = GREET("developer")
say result

when score >= 15 {
    say "condition works"
} otherwise {
    say "not reached"
}

set missing = unknown_value
say "the script continues"
```

الناتج المتوقع هو طباعة اسم اللغة، ثم نتيجة الجذر التربيعي، ثم نتيجة الدالة والشرط. سيظهر تنبيه للمكتبة غير المتوفرة وللاسم غير المعرّف، لكن التنفيذ سيستمر.

## قواعد VSK

| العنصر | الصياغة |
|---|---|
| المتغير | `set name = value` أو `name = value` |
| الإسناد المركب | `name += value`، `name -= value`، `name *= value`، `name /= value` |
| الطباعة | `say expression` |
| الاستيراد | `bring Math` |
| الشرط | `when condition { ... } otherwise { ... }` |
| الحلقة | `repeat condition { ... }` |
| الدالة | `proc name(parameters) { ... }` |
| الإرجاع | `give expression` |
| التعليق | يبدأ بـ `#` أو `//` |
| القيم | أرقام، نصوص، `true`، `false`، `nil` |

## التسامح المقصود

لا تميّز VSK بين الحروف الكبيرة والصغيرة في الكلمات والأسماء؛ لذلك فإن `Name` و`name` و`NAME` اسم واحد. الاستيراد غير المستخدم مسموح ولا يولد تحذيرًا. وعند طلب مكتبة غير متاحة، تطبع النواة تنبيهًا ثم تتابع البرنامج. كذلك لا يوقف الاسم غير المعرّف البرنامج، بل يعادله runtime بقيمة `nil` مع تنبيه.

تظل أخطاء بناء الصياغة، مثل النص غير المغلق أو القوس المفقود، أخطاء حقيقية؛ لأن التسامح مع هذه الأخطاء قد يؤدي إلى تنفيذ برنامج مختلف تمامًا عن قصد الكاتب.

## المكتبات الحالية

النواة تتضمن وحدات منطقية أولية لا تحتاج إلى Python أو Go:

| الوحدة | بعض الأعضاء |
|---|---|
| `Math` | `sqrt`، `abs`، `floor`، `ceil`، `pi` |
| `Text` | `upper`، `lower` |
| `Random` | `number` |

## الاستقلالية ومرحلة self-hosting

توجد مرحلتان مختلفتان يجب عدم الخلط بينهما. في المرحلة الأولى، يستخدم المشروع bootstrap مكتوبًا بلغة C حتى نحصل على مترجم/مفسّر قابل للتشغيل على الجهاز. هذا لا يجعل لغة VSK لغة C، لأن C لا تحدد كلمات VSK أو أنواعها أو سلوكها.

في المرحلة الثانية، وبعد استقرار grammar وruntime، يمكن كتابة مترجم VSK بلغة VSK نفسها. عندها تُترجم VSK ذاتيًا، ويصبح bootstrap C مجرد أداة إقلاع تاريخية. هذه هي مرحلة **self-hosting**.

## الملفات

| الملف | الوظيفة |
|---|---|
| `vsk.c` | نواة VSK: lexer وparser وruntime والتنفيذ |
| `demo_independent.vsk` | مثال باللغة الجديدة |
| `VSK_SPEC.md` | المواصفات والقرارات التصميمية |
| `Makefile` | بناء النواة |
| `vsk` | مشغل مختصر |

## التثبيت من حزمة Debian

بعد تنزيل ملف `vsk_0.3.0_amd64.deb`، ثبّته بالأمر التالي:

```bash
sudo dpkg -i vsk_0.3.0_amd64.deb
```

بعد التثبيت يصبح الأمر متاحًا عالميًا:

```bash
vsk bok.vk
```

كما تتوفر أوامر التحقق التالية:

```bash
vsk --version
vsk --help
```

هذه الحزمة الحالية مبنية لمعمارية `amd64` على Debian/Ubuntu. يمكن إعادة بناء الحزمة لمعمارية أخرى من المصدر باستخدام `packaging/build-deb.sh` بعد توفير مترجم C مناسب لتلك المعمارية.

## روابط النشر

مستودع GitHub:

`https://github.com/xAboodPBx/vsk-language`

صفحة الإصدار `v0.3.0`:

`https://github.com/xAboodPBx/vsk-language/releases/tag/v0.3.0`

رابط تنزيل حزمة Debian amd64:

`https://github.com/xAboodPBx/vsk-language/releases/download/v0.3.0/vsk_0.3.0_amd64.deb`
