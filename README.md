<html>

<head>
<meta http-equiv=Content-Type content="text/html; charset=unicode">
<meta name=Generator content="Microsoft Word 14 (filtered)">
<style>
<!--
 /* Font Definitions */
 @font-face
	{font-family:Wingdings;
	panose-1:5 0 0 0 0 0 0 0 0 0;}
@font-face
	{font-family:"Cambria Math";
	panose-1:2 4 5 3 5 4 6 3 2 4;}
@font-face
	{font-family:Calibri;
	panose-1:2 15 5 2 2 2 4 3 2 4;}
@font-face
	{font-family:Tahoma;
	panose-1:2 11 6 4 3 5 4 4 2 4;}
 /* Style Definitions */
 p.MsoNormal, li.MsoNormal, div.MsoNormal
	{margin-top:0cm;
	margin-right:0cm;
	margin-bottom:10.0pt;
	margin-left:0cm;
	line-height:115%;
	font-size:11.0pt;
	font-family:"Calibri","sans-serif";}
h2
	{mso-style-link:"Заголовок 2 Знак";
	margin-top:12.0pt;
	margin-right:0cm;
	margin-bottom:3.0pt;
	margin-left:0cm;
	text-indent:42.55pt;
	page-break-after:avoid;
	font-size:14.0pt;
	font-family:"Arial","sans-serif";
	font-style:italic;}
p.MsoBodyText, li.MsoBodyText, div.MsoBodyText
	{mso-style-link:"Основной текст Знак";
	margin:0cm;
	margin-bottom:.0001pt;
	text-align:justify;
	text-indent:35.45pt;
	line-height:150%;
	background:white;
	text-autospace:none;
	font-size:12.0pt;
	font-family:"Times New Roman","serif";
	color:black;}
p.MsoAcetate, li.MsoAcetate, div.MsoAcetate
	{mso-style-link:"Текст выноски Знак";
	margin:0cm;
	margin-bottom:.0001pt;
	font-size:8.0pt;
	font-family:"Tahoma","sans-serif";}
p.MsoListParagraph, li.MsoListParagraph, div.MsoListParagraph
	{margin-right:0cm;
	margin-left:0cm;
	font-size:12.0pt;
	font-family:"Times New Roman","serif";}
span.2
	{mso-style-name:"Заголовок 2 Знак";
	mso-style-link:"Заголовок 2";
	font-family:"Arial","sans-serif";
	font-weight:bold;
	font-style:italic;}
span.a
	{mso-style-name:"Основной текст Знак";
	mso-style-link:"Основной текст";
	font-family:"Times New Roman","serif";
	color:black;
	background:white;}
span.a0
	{mso-style-name:"Текст выноски Знак";
	mso-style-link:"Текст выноски";
	font-family:"Tahoma","sans-serif";}
p.msochpdefault, li.msochpdefault, div.msochpdefault
	{mso-style-name:msochpdefault;
	margin-right:0cm;
	margin-left:0cm;
	font-size:12.0pt;
	font-family:"Calibri","sans-serif";}
p.msopapdefault, li.msopapdefault, div.msopapdefault
	{mso-style-name:msopapdefault;
	margin-right:0cm;
	margin-bottom:10.0pt;
	margin-left:0cm;
	line-height:115%;
	font-size:12.0pt;
	font-family:"Times New Roman","serif";}
span.pl-c
	{mso-style-name:pl-c;}
.MsoChpDefault
	{font-size:10.0pt;}
.MsoPapDefault
	{margin-bottom:10.0pt;
	line-height:115%;}
@page WordSection1
	{size:595.3pt 841.9pt;
	margin:2.0cm 42.5pt 2.0cm 3.0cm;}
div.WordSection1
	{page:WordSection1;}
 /* List Definitions */
 ol
	{margin-bottom:0cm;}
ul
	{margin-bottom:0cm;}
-->
</style>

</head>

<body lang=RU>

<div class=WordSection1>

<h2>Что такое система <span lang=EN-US>UDT</span>?</h2>

<p class=MsoBodyText style='line-height:normal'>Система <span lang=EN-US>UDT</span><span
lang=EN-US> </span>предназначена для обмена данными между взаимосвязанными
программами, работающих как на одной ЭВМ так и на разных, приёма и передачи
данных по каналам&nbsp; <span lang=EN-US>RS</span> 485(<span lang=EN-US>RS</span>422,
<span lang=EN-US>RS</span>232), <span lang=EN-US>UDP</span>, <span lang=EN-US>TCP</span>,
<span lang=EN-US>UnixSocket</span>, <span lang=EN-US>Fifo</span>(<span
lang=EN-US>Pipe</span>), <span lang=EN-US>SharedMemory</span>, <span
lang=EN-US>QnxMessage</span>, &nbsp;автоматической маршрутизации пакетов разных
протоколов приёма-передачи данных, их проверку и преобразование в &nbsp;формат <span
lang=EN-US>XML</span>(<span lang=EN-US>Json</span>), резервирование каналов
передачи данных, оптимизации трафика, отладки взаимодействия ПО. Или, проще
говоря, <span lang=EN-US>UDT</span><span lang=EN-US> </span>облегчает отладку
обмена данными между &nbsp;несколькими программами. &nbsp;</p>

<h2>Принцип работы системы.</h2>

<p class=MsoBodyText style='text-indent:0cm;line-height:normal'>Рассмотрим
принцип работы на следующем примере.</p>

<p class=MsoBodyText style='text-indent:35.4pt;line-height:normal'>Допустим
есть 3 программы А, В и С расположенные на разных ЭВМ. Программа А должна&nbsp;
отправлять сообщение №1 программе В и сообщение №2 программе С. Все три
программы обмениваются информацией по некоторому протоколу «Протокол». </p>

<p class=MsoBodyText style='line-height:normal'>Для понимания принципа
построения система, рассмотрим вначале реализацию&nbsp; «в лоб» например с
использованием <span lang=EN-US>UDP</span>. Разработчики программ &nbsp;С, <span
lang=EN-US>B</span><span lang=EN-US> </span>создают сокет и открывают <span
lang=EN-US>udp</span> порт&nbsp; номер 0<span lang=EN-US>x</span>С и 0<span
lang=EN-US>xB</span><span lang=EN-US> </span>соответственно. Далее
разрабатывают функцию, обрабатывающую наш протокол.&nbsp; Разработчик программы
<span lang=EN-US>A</span><span lang=EN-US> </span>открывает порт 0<span
lang=EN-US>xA</span>, далее, при необходимости, отправляет сообщение №1
программе <span lang=EN-US>B</span> и сообщение №2 программе №1. В случае
неизменной архитектуры взаимодействия ПО проблемы с таким &nbsp;подходом не
возникают. Усложним систему, допустим через полгода после разработки программы
А, понадобилось дополнительно отправлять сообщение №1 в новую программу <span
lang=EN-US>D</span>. Тогда в программе А&nbsp; необходимо изменить функцию
отправляющую сообщение №1. А вы уверены, что после этого исправления она будет
работать? Конечно, нет. Получается что, для добавления нового приходить
исправлять старое. И в этом проблема. Причём под новым понимается не только новая
программа, но и, например: новое сообщение, новый интерфейс передачи данных(<span
lang=EN-US>UDP</span>,<span lang=EN-US>TCP</span>, <span lang=EN-US>RS</span><span
lang=EN-US> </span>&nbsp;и т.д.), новая версия протокола, новая архитектура и
т.д. А если понадобилось резервирование обмена данными или разработчик
программы банально уволился, то проблемы с передачей данных начинают
увеличиваться в геометрической прогрессии. </p>

<p class=MsoBodyText style='line-height:normal'>Система <span lang=EN-US>UDT</span><span
lang=EN-US> </span>построена иначе: вместо принципа программа А должна&nbsp;
отправлять&nbsp; сообщение №1 программе В, используется принцип программа В
должна получать сообщение №1 от программы А. Таким образом, в приведённом
примере, для передачи сообщения №1 программе <span lang=EN-US>D</span>, нет
необходимости изменять программу А, достаточно дополнительно «указать», что
программа <span lang=EN-US>D</span> должна получать сообщение №1 от программы
А. Если не понятно, то см. схему ниже. Как видно из схемы в системе <span
lang=EN-US>UDT</span><span lang=EN-US> </span>есть ядро, которое отвечает за
рассылку сообщений между программами. &nbsp;&nbsp;Очевидно, что невозможно
&nbsp;управлять рассылкой сообщений между программами, если нет данных об их
идентификаторах и информации о протоколе передачи данных между ними.</p>

<p class=MsoBodyText style='line-height:normal'>&nbsp;</p>

<p class=MsoBodyText style='text-indent:14.2pt;line-height:normal'><img
width=579 height=173 src="doc/AboutUDT.files/image001.jpg"></p>

<p class=MsoBodyText style='line-height:normal'><b>&nbsp;</b></p>

<p class=MsoBodyText style='line-height:normal'><b>&nbsp;</b></p>

<h2>Идентификатор ПО</h2>

<p class=MsoBodyText style='line-height:normal'>&nbsp;</p>

<p class=MsoBodyText style='line-height:normal'>В системе <span lang=EN-US>UDT</span><span
lang=EN-US> </span>каждой пользовательской программе соответствует два
идентификатора. Первый – это уникальный номер (далее <span lang=EN-US>UUID</span>),
присваиваемый системой <span lang=EN-US>UDT</span><span lang=EN-US> </span>программе
при старте. Основное его назначение – это&nbsp; позволить исполнять
одновременно нескольких копий ПО. Для этого номер генерируется по случайному
закону, поэтому одной и той же копии ПО соответствуют разные номера. Чтобы
гарантировать уникальность <span lang=EN-US>UUID</span>, в его номер можно
добавить детерминированное число. Оно задаётся в&nbsp; настроечном файле.
&nbsp;Второй идентификатор – это идентификатор пользовательской программы
(далее имя ПО). Он представляет собой строку с кодировкой <span lang=EN-US>utf</span>8,
задаваемой пользователем (см. модуль «динамическая библиотека пользователя» или
настроечный файл тэг «rrdname»). Имя ПО &nbsp;может состоять из любых символов <span
lang=EN-US>Unicode</span> в нижнем регистре кроме «@» и «.». Символ @
используется для задание подгрупп пользовательского ПО в следующем формате:
«имя@подгрупа1.подгрупа2.подгрупа3…». Проще говоря, имя имеет формат
электронной почты, за исключением того имя домена записывается слева направо.
Например, если в качестве идентификатора ПО использовать электронную почту «<span
class=pl-c><span lang=EN-US>UDT</span>@gmail.com», то его имя в системе </span><span
class=pl-c><span lang=EN-US>UDT</span><span lang=EN-US> </span>записывается как
«</span><span class=pl-c><span lang=EN-US>UDT</span>@com.gmail».</span> Если
две пользовательские программы принадлежат разным подгруппам, то обмен данными
между ними ограничен (см. модуль «динамическая библиотека пользователя).
Например, ПО с именем «программа_а@группа_а»&nbsp; может взаимодействовать с ПО
«программа_<span lang=EN-US>b</span>@группа_<span lang=EN-US>a</span>», а с ПО
«программа_<span lang=EN-US>b</span>@группа_<span lang=EN-US>b</span>» нет. </p>

<p class=MsoBodyText style='line-height:normal'><b>&nbsp;</b></p>

<h2>Протокол передачи данных между программами</h2>

<p class=MsoBodyText style='line-height:normal'>&nbsp;</p>

<p class=MsoBodyText style='line-height:normal'>В системе <span lang=EN-US>UDT</span><span
lang=EN-US> </span>пользователю доступно два типа протокола. Первый – встроенный
протокол передачи данных, его идентификатор «<span lang=EN-US>raw</span>». В
этом протоколе сообщения различаются только идентификационным номером,
задаваемым пользователем. Поскольку &nbsp;этот протокол встроенный, то номер
сообщения &nbsp;передаётся отдельно от самого сообщения. Т.е. в самом сообщении
номер нигде не хранится.&nbsp; Второй тип протоколов – это внешние протоколы,
созданные пользователем (см модуль «обработки прокола обмена данными»).
&nbsp;Поскольку эти протоколы внешние по отношению к системе, то сообщения
должны содержать в себе идентифицирующую информацию. Или более простым языком,
если пользователь добавляет свой протокол, то система <span lang=EN-US>UDT</span><span
lang=EN-US> </span>должна иметь возможность найти сообщение в буфере данных, а
это сделать невозможно, если в сообщении не будет содержаться идентифицирующая
информация. Со встроенным протоколом ситуация иная, система <span lang=EN-US>UDT</span><span
lang=EN-US> </span>знает какой размер сообщения передаётся и где оно
располагается, единственное чего не хватает &nbsp;это идентификатора, который
задаёт пользователь. </p>

<p class=MsoBodyText style='line-height:normal'>&nbsp;</p>

<h2>Инструкция по установке</h2>

<p class=MsoNormal style='margin-top:12.0pt;margin-right:0cm;margin-bottom:
0cm;margin-left:0cm;margin-bottom:.0001pt;text-indent:35.4pt'><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>Система
сборки </span><span lang=EN-US style='font-size:12.0pt;line-height:115%;
font-family:"Times New Roman","serif"'>UDT</span><span lang=EN-US
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>
</span><span style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>&nbsp;построена
на основе автоматического сборщика проектов </span><span lang=EN-US
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>cmake</span><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>.
Перед началом установки, рекомендуется с ним ознакомиться. Далее в инструкции
будет использоваться </span><span lang=EN-US style='font-size:12.0pt;
line-height:115%;font-family:"Times New Roman","serif"'>gui</span><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>
</span><span style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>версия
</span><span lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:
"Times New Roman","serif"'>cmake</span><span style='font-size:12.0pt;
line-height:115%;font-family:"Times New Roman","serif"'>.</span></p>

<p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;text-indent:
35.4pt'><i><span style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>Напоминаю
</span></i><i><span lang=EN-US style='font-size:12.0pt;line-height:115%;
font-family:"Times New Roman","serif"'>cmake</span></i><i><span lang=EN-US
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>
</span></i><i><span style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>штука
капризная, поэтому во избежание&nbsp; проблем, рекомендуется почистить&nbsp;
переменные окружения, оставляя только каталоги целевого компилятора, в первую
очередь это касается переменной </span></i><i><span lang=EN-US
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>PATH</span></i><i><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>.
&nbsp;(Например, если вы собираете проект для </span></i><i><span lang=EN-US
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>mingw</span></i><i><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>,
то не редко </span></i><i><span lang=EN-US style='font-size:12.0pt;line-height:
115%;font-family:"Times New Roman","serif"'>cmake</span></i><i><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>
</span></i><i><span style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>«подхватывает»
</span></i><i><span lang=EN-US style='font-size:12.0pt;line-height:115%;
font-family:"Times New Roman","serif"'>sh</span></i><i><span style='font-size:
12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>.</span></i><i><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>exe</span></i><i><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>
</span></i><i><span lang=EN-US style='font-size:12.0pt;line-height:115%;
font-family:"Times New Roman","serif"'>&nbsp;</span></i><i><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>от
другого компилятора). </span></i></p>

<p class=MsoNormal><span style='font-family:"Times New Roman","serif"'>&nbsp;</span></p>

<p class=MsoNormal style='text-indent:35.4pt'><b><span style='font-size:12.0pt;
line-height:115%;font-family:"Times New Roman","serif"'>Зависимости</span></b><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>:</span></p>

<table class=MsoNormalTable border=0 cellspacing=0 cellpadding=0
 style='border-collapse:collapse'>
 <tr style='height:3.5pt'>
  <td width=102 valign=top style='width:76.3pt;border:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:3.5pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt'><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Название</span></p>
  </td>
  <td width=66 valign=top style='width:49.6pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt;height:3.5pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt'><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Версия</span></p>
  </td>
  <td width=180 valign=top style='width:134.65pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt;height:3.5pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt'><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Обязательно</span></p>
  </td>
  <td width=291 valign=top style='width:218.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt;height:3.5pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt'><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Примечание</span></p>
  </td>
 </tr>
 <tr style='height:50.45pt'>
  <td width=102 valign=top style='width:76.3pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Boost</span></p>
  </td>
  <td width=66 valign=top style='width:49.6pt;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>1</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>.47
  </span><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>и
  выше</span></p>
  </td>
  <td width=180 valign=top style='width:134.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Только
  заголовочные файлы (</span><span lang=EN-US style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>Header</span><span lang=EN-US
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'> </span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>only</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>)</span></p>
  </td>
  <td width=291 valign=top style='width:218.0pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Необходимая
  версия уже лежит в каталоге UDT/SHARE/dependencies</span></p>
  </td>
 </tr>
 <tr style='height:50.45pt'>
  <td width=102 valign=top style='width:76.3pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>rapidjson</span></p>
  </td>
  <td width=66 valign=top style='width:49.6pt;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>1.02
  </span><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>и
  выше</span></p>
  </td>
  <td width=180 valign=top style='width:134.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Только
  заголовочные файлы (</span><span lang=EN-US style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>Header</span><span lang=EN-US
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'> </span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>only</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>)</span></p>
  </td>
  <td width=291 valign=top style='width:218.0pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Необходимая
  версия уже лежит в каталоге UDT/SHARE/dependencies</span></p>
  </td>
 </tr>
 <tr style='height:50.45pt'>
  <td width=102 valign=top style='width:76.3pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Tclap</span></p>
  </td>
  <td width=66 valign=top style='width:49.6pt;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>1</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>.2.1
  </span><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>и
  выше</span></p>
  </td>
  <td width=180 valign=top style='width:134.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Только
  заголовочные файлы (</span><span lang=EN-US style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>Header</span><span lang=EN-US
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'> </span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>only</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>)</span></p>
  </td>
  <td width=291 valign=top style='width:218.0pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Необходимая
  версия уже лежит в каталоге UDT/SHARE/dependencies</span></p>
  </td>
 </tr>
 <tr style='height:50.45pt'>
  <td width=102 valign=top style='width:76.3pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Log4cplus</span></p>
  </td>
  <td width=66 valign=top style='width:49.6pt;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>1.1.3
  </span><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>и
  1</span><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>.2.*</span></p>
  </td>
  <td width=180 valign=top style='width:134.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;margin-bottom:.0001pt;
  text-align:center;line-height:normal'><span style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>Нет, если не указана опция </span></p>
  <p class=MsoNormal align=center style='margin-bottom:0cm;margin-bottom:.0001pt;
  text-align:center;line-height:normal'><span style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>LOGGING_TO_LOG4CPLUS</span></p>
  </td>
  <td width=291 valign=top style='width:218.0pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Компилируется
  автоматически, если указана опция: LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE</span></p>
  </td>
 </tr>
 <tr style='height:50.45pt'>
  <td width=102 valign=top style='width:76.3pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Glog</span></p>
  </td>
  <td width=66 valign=top style='width:49.6pt;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>0.3</span></p>
  </td>
  <td width=180 valign=top style='width:134.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;margin-bottom:.0001pt;
  text-align:center;line-height:normal'><span style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>Нет, если не указана опция </span></p>
  <p class=MsoNormal align=center style='margin-bottom:0cm;margin-bottom:.0001pt;
  text-align:center;line-height:normal'><span style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>LOGGING_TO_GLOG</span></p>
  </td>
  <td width=291 valign=top style='width:218.0pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt;height:50.45pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>&nbsp;</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal><span style='font-size:12.0pt;line-height:115%;font-family:
"Times New Roman","serif"'>&nbsp;</span></p>

<p class=MsoNormal style='text-indent:35.4pt'><b><span style='font-size:12.0pt;
line-height:115%;font-family:"Times New Roman","serif"'>Сборка проекта:</span></b></p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Запустите <span lang=EN-US>cmake</span>-<span lang=EN-US>gui</span>;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>В поле «<span lang=EN-US>Where</span><span lang=EN-US> </span><span
lang=EN-US>is</span><span lang=EN-US> </span><span lang=EN-US>the</span><span
lang=EN-US> </span><span lang=EN-US>source</span><span lang=EN-US> </span><span
lang=EN-US>code</span>» укажите путь к исходному коду;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>В поле&nbsp; «<span lang=EN-US>Where</span><span lang=EN-US> </span><span
lang=EN-US>to</span><span lang=EN-US> </span><span lang=EN-US>build</span><span
lang=EN-US> </span><span lang=EN-US>binaries</span>» укажите путь, где будут
находиться скрипты сборки проекта;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Нажмите кнопку «<span lang=EN-US>Configure</span>»;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Далее выбираем компилятор и <span lang=EN-US>IDE</span>;</p>

<p class=MsoListParagraph style='margin-left:18.0pt'><i>Для</i><i> <span
lang=EN-US>OS QNX </span>выбираем</i><i><span lang=EN-US>: unix makefiles </span>и</i><i>
<span lang=EN-US>Specify toolchain file for cross-compiling. </span>Нажимаем</i><i><span
lang=EN-US> «Next», </span>указываем</i><i> путь</i><i> к</i><i> файлу</i><i><span
lang=EN-US> toolchain-QNX-&lt;</span>номер</i><i> версии</i><i><span
lang=EN-US>&gt;.cmake </span>в</i><i> папке</i><i><span lang=EN-US>
UDT/SHARE/CMakeModules/toolchain.</span></i></p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Настраиваем проект, см. пункт Опции сборки;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Нажимаем «<span lang=EN-US>Generate</span>»;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Если всё хорошо, то <span lang=EN-US>cmake</span> должен
сгенерировать проект в папке «<span lang=EN-US>Where</span><span lang=EN-US> </span><span
lang=EN-US>to</span><span lang=EN-US> </span><span lang=EN-US>build</span><span
lang=EN-US> </span><span lang=EN-US>binaries</span>». </p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Если на шаге 5 был выбран <span lang=EN-US>IDE</span>, то заходим
в папку указанную в поле &nbsp;«<span lang=EN-US>Where</span><span lang=EN-US> </span><span
lang=EN-US>to</span><span lang=EN-US> </span><span lang=EN-US>build</span><span
lang=EN-US> </span><span lang=EN-US>binaries</span>» и запускаем проект. Если
был выбран <span lang=EN-US>Makefile</span>, заходим в папку «<span lang=EN-US>Where</span><span
lang=EN-US> </span><span lang=EN-US>to</span><span lang=EN-US> </span><span
lang=EN-US>build</span><span lang=EN-US> </span><span lang=EN-US>binaries</span>»
из командной строки и &nbsp;набираем команду «<span lang=EN-US>make</span><span
lang=EN-US> </span><span lang=EN-US>install</span>»;</p>

<p class=MsoListParagraph style='margin-left:18.0pt;text-indent:-18.0pt'><span
style='font-family:Symbol'>·<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Если всё прошло&nbsp; успешно, то файлы будут лежать в папке
указанной в опции <span lang=EN-US>CMAKE</span>_<span lang=EN-US>INSTALL</span>_<span
lang=EN-US>PREFIX</span>.</p>

<p class=MsoListParagraph><span lang=EN-US>&nbsp;</span></p>

<p class=MsoListParagraph style='text-indent:18.0pt'><b>Опции сборки</b><b><span
lang=EN-US>:</span></b></p>

<table class=MsoNormalTable border=0 cellspacing=0 cellpadding=0 width=616
 style='width:462.3pt;margin-left:18.0pt;border-collapse:collapse'>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Название</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Описание</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>*
  _WITH_STATIC_DEPENDENCIES</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Компилировать
  проект со статическими зависимостями.</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>*</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>_</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>BUILD_STATIC_TOO</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Дополнительно
  собрать статическую библиотеку</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>CUSTOMER_AVAILABLE_MODULES</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Список
  существующих дополнительных модулей</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>CUSTOMER_WITH_STATIC_MODULES</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Собрать
  библиотеку </span><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>customer</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'> </span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>вместе с
  модулями указанными в </span><span lang=EN-US style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>CUSTOMER</span><span style='font-size:
  12.0pt;font-family:"Times New Roman","serif"'> _</span><span lang=EN-US
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LIST</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>_</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>STATIC</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>_</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>MODULES</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Также
  собрать </span><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>log4cplus
  1.2</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LOGGING_HAS_TO_BE_REMOVED</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Удалить
  все логи и ассерты из исходного кода</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LOGGING_IS_DISABLED_IN_SHARE</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Отключить
  логи только в библиотеки </span><span lang=EN-US style='font-size:12.0pt;
  font-family:"Times New Roman","serif"'>SHARE</span><span style='font-size:
  12.0pt;font-family:"Times New Roman","serif"'>, но ассерты оставить</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LOGGING_TO_LOG4CPLUS</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Для
  логирования использовать библиотеку </span><span lang=EN-US style='font-size:
  12.0pt;font-family:"Times New Roman","serif"'>log</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>4</span><span
  lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>cplus</span><span
  style='font-size:12.0pt;font-family:"Times New Roman","serif"'>. <i>Рекомендуется
  устанавливать эту опцию.</i></span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span lang=EN-US style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LOGGING_TO_GLOG</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Для
  логирования использовать библиотеку </span><span lang=EN-US style='font-size:
  12.0pt;font-family:"Times New Roman","serif"'>glog</span></p>
  </td>
 </tr>
 <tr>
  <td width=276 valign=top style='width:207.15pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>LOGGING_TO_COUT</span></p>
  </td>
  <td width=340 valign=top style='width:9.0cm;border-top:none;border-left:none;
  border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;margin-bottom:.0001pt;line-height:
  normal'><span style='font-size:12.0pt;font-family:"Times New Roman","serif"'>Выводить
  логи в стандартный поток вывода </span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal style='margin-left:18.0pt'><span style='font-size:12.0pt;
line-height:115%;font-family:"Times New Roman","serif"'>Обычно достаточно
настроить&nbsp; только логирование проекта. &nbsp;Для логирования&nbsp;
рекомендуется использовать библиотеку </span><span lang=EN-US style='font-size:
12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>log</span><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>4</span><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>cplus</span><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>
опция </span><span lang=EN-US style='font-size:12.0pt;line-height:115%;
font-family:"Times New Roman","serif"'>LOGGING</span><span style='font-size:
12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>_</span><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>TO</span><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>_</span><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>LOG</span><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>4</span><span
lang=EN-US style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>CPLUS</span><span
style='font-size:12.0pt;line-height:115%;font-family:"Times New Roman","serif"'>,
если библиотека в системе не установлена, то при установке опции
LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE она откомпилируется автоматически и
слинкуется статически.</span></p>

<p class=MsoNormal style='margin-left:18.0pt'>&nbsp;</p>

<p class=MsoNormal style='margin-left:18.0pt'>&nbsp;</p>

</div>

</body>

</html>
