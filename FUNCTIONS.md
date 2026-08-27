# دليل دوال وأوامر VSK

هذا الدليل يشرح طريقة كتابة البرامج في VSK واستعمال الدوال الجاهزة وكتابة الدوال الخاصة

## تشغيل ملف

احفظ البرنامج في ملف بامتداد `.vk` ثم شغله من الطرفية

```bash
vsk bok.vk
```

مثال بسيط

```vsk
say "Hello from VSK"
```

## أمر say

يستخدم الأمر `say` لطباعة قيمة واحدة

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

## المتغيرات والإسناد

يستخدم الأمر `set` لإنشاء متغير

```vsk
set name = "VSK"
set score = 10
say name
say score
```

يمكن تغيير المتغير دون كتابة `set`

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

## كتابة دالة خاصة

تكتب الدالة باستخدام `proc` ثم اسم الدالة ثم المعاملات داخل الأقواس

```vsk
proc add(first, second) {
    give first + second
}

set result = add(4, 6)
say result
```

الكلمة `give` تعيد قيمة من الدالة

```vsk
proc welcome(person) {
    say "Welcome " + person
    give true
}

set accepted = welcome("Ali")
say accepted
```

يمكن استدعاء دالة داخل دالة أخرى

```vsk
proc double(value) {
    give value * 2
}

proc quadruple(value) {
    give double(double(value))
}

say quadruple(3)
```

## وحدة Math

استورد الوحدة باستخدام `bring Math`

```vsk
bring Math
```

| العضو | الاستخدام | مثال |
|---|---|---|
| `Math.sqrt(value)` | حساب الجذر التربيعي | `Math.sqrt(81)` |
| `Math.abs(value)` | حساب القيمة المطلقة | `Math.abs(-8)` |
| `Math.floor(value)` | التقريب إلى الأسفل | `Math.floor(4.9)` |
| `Math.ceil(value)` | التقريب إلى الأعلى | `Math.ceil(4.1)` |
| `Math.pi` | قيمة باي | `Math.pi` |

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

استورد الوحدة باستخدام `bring Text`

```vsk
bring Text
```

| الدالة | الاستخدام | مثال |
|---|---|---|
| `Text.upper(value)` | تحويل النص إلى حروف كبيرة | `Text.upper("hello")` |
| `Text.lower(value)` | تحويل النص إلى حروف صغيرة | `Text.lower("HELLO")` |

مثال

```vsk
bring Text

say Text.upper("hello vsk")
say Text.lower("HELLO VSK")
```

## وحدة Random

استورد الوحدة باستخدام `bring Random`

```vsk
bring Random
```

| الدالة | الاستخدام | مثال |
|---|---|---|
| `Random.number()` | إنشاء رقم عشوائي بين صفر وواحد | `Random.number()` |

مثال

```vsk
bring Random
say Random.number()
```

## الشروط

يستخدم `when` لتنفيذ أوامر عند تحقق الشرط

```vsk
set age = 21

when age >= 18 {
    say "adult"
} otherwise {
    say "minor"
}
```

العمليات المنطقية هي `and` و`or` و`not`

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

يستخدم `repeat` لإعادة تنفيذ الأوامر طالما بقي الشرط صحيحًا

```vsk
set counter = 0

repeat counter < 5 {
    say counter
    counter += 1
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

جمع النصوص يتم باستخدام `+`

```vsk
set first = "VSK"
set second = " language"
say first + second
```

## الاستيراد المرن

يمكن طلب وحدة غير موجودة وسيظهر تنبيه ثم يستمر البرنامج

```vsk
bring UnknownLibrary
say "the program continues"
```

إذا استعملت اسمًا غير معرف ستعيد VSK القيمة `nil` مع تنبيه

```vsk
say unknown_value
say "the program continues"
```

## مثال كامل

احفظ المثال التالي في ملف باسم `bok.vk`

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

شغل المثال باستخدام

```bash
vsk bok.vk
```
