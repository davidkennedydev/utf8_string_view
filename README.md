# utf8_string_view
A string_view addressed to UTF-8 encoded characters.

Common **string view** not treat UTF-8 encoding at this way we take incorrect string length (based on _code units_) and iterate by bytes (_code units_).

This **utf8_string_view** type can treat UTF-8 enconding and take correct length (based on _code points_) and iterate by character (_code points_).

```
One byte character:   a
Two byte character:   ç
Three byte character: ⊞
Four byte character:  🠶

Common string view:
Length: 6		 Data: 1🠶3
Each character iteration (foreach): 	1 � � � � 3
Each character iteration (for index): 	1 � � � � 3

UTF-8 string view:
Length: 3		 Data: 1🠶3
Each character iteration (foreach): 	1 🠶 3
Each character iteration (for index): 	1 🠶 3

```
