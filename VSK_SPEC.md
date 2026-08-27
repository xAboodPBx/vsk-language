# مواصفات لغة VSK

## التعريف

VSK لغة برمجية خفيفة لكتابة السكربتات بطريقة واضحة ومرنة

تقرأ VSK الملف البرمجي بامتداد `.vk` ثم تنفذ أوامره بالترتيب

## الكلمات الأساسية

| الكلمة | الوظيفة |
|---|---|
| `set` | إنشاء متغير أو تغيير قيمته |
| `say` | طباعة قيمة |
| `bring` | استدعاء وحدة |
| `when` | تنفيذ شرط |
| `otherwise` | تنفيذ الفرع البديل |
| `repeat` | تكرار الأوامر حسب شرط |
| `proc` | تعريف دالة |
| `give` | إعادة قيمة من دالة |
| `true` | قيمة صحيحة |
| `false` | قيمة خاطئة |
| `nil` | قيمة فارغة |

## المتغيرات

```vsk
set name = "VSK"
set number = 10
number += 5
say name
say number
```

لا تفرق VSK بين الحروف الكبيرة والصغيرة في أسماء المتغيرات والكلمات الأساسية

## الشروط والحلقات

```vsk
when number > 10 {
    say "greater"
} otherwise {
    say "smaller or equal"
}

set counter = 0
repeat counter < 3 {
    say counter
    counter += 1
}
```

## الدوال

```vsk
proc add(first, second) {
    give first + second
}

say add(4, 6)
```

## الوحدات

```vsk
bring Math
say Math.sqrt(81)
```

الوحدات المتاحة هي `Math` و`Text` و`Random`

للدليل الكامل راجع [FUNCTIONS.md](FUNCTIONS.md)
