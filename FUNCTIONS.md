# دليل أوامر ودوال VSK

هذا الدليل يشرح طريقة استخدام أوامر VSK والدوال الجاهزة وكتابة دوالك الخاصة

## تشغيل ملف VSK

احفظ البرنامج داخل ملف بامتداد `.vk` ثم شغله من الطرفية

```bash
vsk bok.vk
```

مثال الملف

```vsk
say "Hello from VSK"
```

## أمر say

يستخدم `say` لطباعة قيمة واحدة إلى الشاشة

```vsk
say "hello"
say 25
say true
say nil
```

يمكن طباعة نتيجة دالة مباشرة

```vsk
bring Math
say Math.sqrt(81)
```

## أمر set والإسناد

يستخدم `set` لإنشاء متغير أو إعطائه قيمة

```vsk
set name = "VSK"
set score = 10
say name
say score
```

يمكن تعديل المتغير بدون كتابة `set`

```vsk
score = 20
score += 5
score -= 2
score *= 2
score /= 4
say score
```

لا تفرق VSK بين الحروف الكبيرة والصغيرة في أسماء المتغيرات

```vsk
set UserName = "Ali"
say username
say USERNAME
```

## الدوال التي يكتبها المستخدم

تكتب الدالة باستخدام `proc` ثم اسم الدالة ثم المعاملات داخل الأقواس

```vsk
proc add(first, second) {
    give first + second
}

set result = add(4, 6)
say result
```

الكلمة `give` تعيد النتيجة إلى المكان الذي استدعى الدالة

```vsk
proc welcome(person) {
    say "Welcome " + person
    give true
}

set accepted = welcome("Ali")
say accepted
```

يمكن للدالة استدعاء دالة أخرى

```vsk
proc double(value) {
    give value * 2
}

proc quadruple(value) {
    give double(double(value))
}

say quadruple(3)
```

يمكن استخدام الشروط داخل الدالة

```vsk
proc check_score(score) {
    when score >= 50 {
        give "passed"
    } otherwise {
        give "failed"
    }
}

say check_score(70)
```

يمكن استخدام الدالة بشكل تكراري

```vsk
proc factorial(number) {
    when number <= 1 {
        give 1
    } otherwise {
        give number * factorial(number - 1)
    }
}

say factorial(5)
```

## وحدة Math

استورد الوحدة قبل استخدامها

```vsk
bring Math
```

| الدالة | الاستخدام | النتيجة |
|---|---|---:|
| `Math.sqrt(value)` | الجذر التربيعي | `Math.sqrt(81)` تعيد `9` |
| `Math.abs(value)` | القيمة المطلقة | `Math.abs(-8)` تعيد `8` |
| `Math.floor(value)` | تقريب إلى الأسفل | `Math.floor(4.9)` تعيد `4` |
| `Math.ceil(value)` | تقريب إلى الأعلى | `Math.ceil(4.1)` تعيد `5` |
| `Math.pi` | قيمة باي | قيمة عددية ثابتة |

مثال

```vsk
bring Math

say Math.sqrt(144)
say Math.abs(-12)
say Math.floor(5.99)
say Math.ceil(5.01)
say Math.pi
```

## وحدة Text

استورد الوحدة قبل استخدامها

```vsk
bring Text
```

| الدالة | الاستخدام | النتيجة |
|---|---|---|
| `Text.upper(value)` | تحويل النص إلى حروف كبيرة | `HELLO` |
| `Text.lower(value)` | تحويل النص إلى حروف صغيرة | `hello` |

مثال

```vsk
bring Text

say Text.upper("hello vsk")
say Text.lower("HELLO VSK")
```

## وحدة Random

استورد الوحدة قبل استخدامها

```vsk
bring Random
```

| الدالة | الاستخدام |
|---|---|
| `Random.number()` | إنتاج رقم عشوائي بين صفر وواحد |

مثال

```vsk
bring Random

say Random.number()
```

## الشروط

يستخدم `when` لتنفيذ مجموعة أوامر عندما يكون الشرط صحيحًا

```vsk
set age = 21

when age >= 18 {
    say "adult"
} otherwise {
    say "minor"
}
```

العمليات المنطقية المتاحة هي `and` و`or` و`not`

```vsk
set age = 25
set has_card = true

when age >= 18 and has_card {
    say "access granted"
}

when not has_card {
    say "card required"
}
```

## الحلقات

يستخدم `repeat` لإعادة التنفيذ طالما كان الشرط صحيحًا

```vsk
set counter = 0

repeat counter < 5 {
    say counter
    counter += 1
}
```

تتوقف الحلقة عند تحول الشرط إلى `false`

```vsk
set value = 3

repeat value > 0 {
    say value
    value -= 1
}
```

## القيم والعمليات

تدعم VSK الأرقام والنصوص والقيم المنطقية والقيمة الفارغة

```vsk
set number = 12
set text = "VSK"
set enabled = true
set empty = nil
```

العمليات الحسابية

```vsk
say 2 + 3
say 8 - 3
say 4 * 5
say 20 / 4
say 10 % 3
```

عمليات المقارنة

```vsk
say 5 == 5
say 5 != 3
say 5 > 3
say 5 >= 5
say 2 < 4
say 2 <= 2
```

يمكن جمع النصوص معًا باستخدام `+`

```vsk
set first = "VSK"
set second = " language"
say first + second
```

## المكتبات غير الموجودة والأسماء غير المعرفة

إذا طلب البرنامج مكتبة غير موجودة تظهر رسالة تنبيه ويستمر البرنامج

```vsk
bring UnknownLibrary
say "the program continues"
```

إذا استُخدم اسم غير معرف تعيده VSK كقيمة `nil` مع رسالة تنبيه

```vsk
say unknown_value
say "the program continues"
```

هذا السلوك مفيد للسكربتات المرنة لكنه يعني أن عليك مراجعة التنبيهات عند بناء برنامج مهم

## مثال تطبيقي كامل

أنشئ ملفًا باسم `bok.vk`

```vsk
bring Math
bring Text

proc calculate(value) {
    give Math.sqrt(value) + 10
}

set name = "vsk user"
set number = 81

say Text.upper(name)
say calculate(number)

when number >= 80 {
    say "high value"
} otherwise {
    say "low value"
}
```

ثم شغله

```bash
vsk bok.vk
```
