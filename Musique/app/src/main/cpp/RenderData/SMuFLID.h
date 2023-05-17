#ifndef MUSIQUE_SMUFLID_H
#define MUSIQUE_SMUFLID_H

#include "../MusicData/Types.h"

/**
 * An enum that maps SMuFL glyph names to SMuFL code points.
 */
enum class SMuFLID : uint16_t
{
    None = 0,
    ErrorGlyph = 0xEE06, // just a random glyph (for debugging)

    repeatLeft = 0xE040,
    repeatRight = 0xE041,
    repeatRightLeft = 0xE042,
    repeatDots = 0xE043,
    repeatDot = 0xE044,
    dalSegno = 0xE045,
    daCapo = 0xE046,
    segno = 0xE047,
    coda = 0xE048,
    codaSquare = 0xE049,
    segnoSerpent1 = 0xE04A,
    segnoSerpent2 = 0xE04B,
    leftRepeatSmall = 0xE04C,
    rightRepeatSmall = 0xE04D,

    GClef = 0xE050,
    GClef8vb = 0xE052,
    GClef8va = 0xE053,
    FClef = 0xE062,
    CClef = 0xE05C,
    StringTabClef6 = 0xE06D,
    StringTabClef4 = 0xE06E,
    unpitchedPercussionClef1 = 0xE069,

    timeSig0 = 0xE080,
    timeSig1 = 0xE081,
    timeSig2 = 0xE082,
    timeSig3 = 0xE083,
    timeSig4 = 0xE084,
    timeSig5 = 0xE085,
    timeSig6 = 0xE086,
    timeSig7 = 0xE087,
    timeSig8 = 0xE088,
    timeSig9 = 0xE089,
    timeSigCommon = 0xE08A,
    timeSigCutCommon = 0xE08B,

    // Noteheads
    noteheadDoubleWhole = 0xE0A0,
    noteheadDoubleWholeSquare = 0xE0A1,
    noteheadWhole = 0xE0A2,
    noteheadHalf = 0xE0A3,
    noteheadBlack = 0xE0A4,
    noteheadNull = 0xE0A5,
    noteheadXDoubleWhole = 0xE0A6,
    noteheadXWhole = 0xE0A7,
    noteheadXHalf = 0xE0A8,
    noteheadXBlack = 0xE0A9,
    noteheadXOrnate = 0xE0AA,
    noteheadXOrnateEllipse = 0xE0AB,
    noteheadPlusDoubleWhole = 0xE0AC,
    noteheadPlusWhole = 0xE0AD,
    noteheadPlusHalf = 0xE0AE,
    noteheadPlusBlack = 0xE0AF,
    noteheadCircleXDoubleWhole = 0xE0B0,
    noteheadCircleXWhole = 0xE0B1,
    noteheadCircleXHalf = 0xE0B2,
    noteheadCircleX = 0xE0B3,
    noteheadDoubleWholeWithX = 0xE0B4,
    noteheadWholeWithX = 0xE0B5,
    noteheadHalfWithX = 0xE0B6,
    noteheadVoidWithX = 0xE0B7,
    noteheadSquareWhite = 0xE0B8,
    noteheadSquareBlack = 0xE0B9,
    noteheadTriangleUpDoubleWhole = 0xE0BA,
    noteheadTriangleUpWhole = 0xE0BB,
    noteheadTriangleUpHalf = 0xE0BC,
    noteheadTriangleUpWhite = 0xE0BD,
    noteheadTriangleUpBlack = 0xE0BE,
    noteheadTriangleLeftWhite = 0xE0BF,
    noteheadTriangleLeftBlack = 0xE0C0,
    noteheadTriangleRightWhite = 0xE0C1,
    noteheadTriangleRightBlack = 0xE0C2,
    noteheadTriangleDownDoubleWhole = 0xE0C3,
    noteheadTriangleDownWhole = 0xE0C4,
    noteheadTriangleDownHalf = 0xE0C5,
    noteheadTriangleDownWhite = 0xE0C6,
    noteheadTriangleDownBlack = 0xE0C7,
    noteheadTriangleUpRightWhite = 0xE0C8,
    noteheadTriangleUpRightBlack = 0xE0C9,
    noteheadMoonWhite = 0xE0CA,
    noteheadMoonBlack = 0xE0CB,
    noteheadTriangleRoundDownWhite = 0xE0CC,
    noteheadTriangleRoundDownBlack = 0xE0CD,
    noteheadParenthesis = 0xE0CE,
    noteheadSlashedBlack1 = 0xE0CF,
    noteheadSlashedBlack2 = 0xE0D0,
    noteheadSlashedHalf1 = 0xE0D1,
    noteheadSlashedHalf2 = 0xE0D2,
    noteheadSlashedWhole1 = 0xE0D3,
    noteheadSlashedWhole2 = 0xE0D4,
    noteheadSlashedDoubleWhole1 = 0xE0D5,
    noteheadSlashedDoubleWhole2 = 0xE0D6,
    noteheadDiamondDoubleWhole = 0xE0D7,
    noteheadDiamondWhole = 0xE0D8,
    noteheadDiamondHalf = 0xE0D9,
    noteheadDiamondHalfWide = 0xE0DA,
    noteheadDiamondBlack = 0xE0DB,
    noteheadDiamondBlackWide = 0xE0DC,
    noteheadDiamondWhite = 0xE0DD,
    noteheadDiamondWhiteWide = 0xE0DE,
    noteheadDiamondDoubleWholeOld = 0xE0DF,
    noteheadDiamondWholeOld = 0xE0E0,
    noteheadDiamondHalfOld = 0xE0E1,
    noteheadDiamondBlackOld = 0xE0E2,
    noteheadDiamondHalfFilled = 0xE0E3,
    noteheadCircledBlack = 0xE0E4,
    noteheadCircledHalf = 0xE0E5,
    noteheadCircledWhole = 0xE0E6,
    noteheadCircledDoubleWhole = 0xE0E7,
    noteheadCircledBlackLarge = 0xE0E8,
    noteheadCircledHalfLarge = 0xE0E9,
    noteheadCircledWholeLarge = 0xE0EA,
    noteheadCircledDoubleWholeLarge = 0xE0EB,
    noteheadCircledXLarge = 0xE0EC,
    noteheadLargeArrowUpDoubleWhole = 0xE0ED,
    noteheadLargeArrowUpWhole = 0xE0EE,
    noteheadLargeArrowUpHalf = 0xE0EF,
    noteheadLargeArrowUpBlack = 0xE0F0,
    noteheadLargeArrowDownDoubleWhole = 0xE0F1,
    noteheadLargeArrowDownWhole = 0xE0F2,
    noteheadLargeArrowDownHalf = 0xE0F3,
    noteheadLargeArrowDownBlack = 0xE0F4,
    noteheadParenthesisLeft = 0xE0F5,
    noteheadParenthesisRight = 0xE0F6,
    noteheadCircleSlash = 0xE0F7,
    noteheadHeavyX = 0xE0F8,
    noteheadHeavyXHat = 0xE0F9,
    noteheadWholeFilled = 0xE0FA,
    noteheadHalfFilled = 0xE0FB,
    noteheadDiamondOpen = 0xE0FC,

    // Slash noteheads
    noteheadSlashVerticalEnds = 0xE100,
    noteheadSlashHorizontalEnds = 0xE101,
    noteheadSlashWhiteWhole = 0xE102,
    noteheadSlashWhiteHalf = 0xE103,
    noteheadSlashDiamondWhite = 0xE104,
    noteheadSlashVerticalEndsSmall = 0xE105,
    noteheadSlashX = 0xE106,
    noteheadSlashVerticalEndsMuted = 0xE107,
    noteheadSlashHorizontalEndsMuted = 0xE108,
    noteheadSlashWhiteMuted = 0xE109,
    noteheadSlashWhiteDoubleWhole = 0xE10A,

    // Round and square noteheads
    noteheadRoundBlackLarge = 0xE110,
    noteheadRoundWhiteLarge = 0xE111,
    noteheadRoundWhiteWithDotLarge = 0xE112,
    noteheadRoundBlack = 0xE113,
    noteheadRoundWhite = 0xE114,
    noteheadRoundWhiteWithDot = 0xE115,
    noteheadRoundBlackSlashedLarge = 0xE116,
    noteheadRoundWhiteSlashedLarge = 0xE117,
    noteheadRoundBlackSlashed = 0xE118,
    noteheadRoundWhiteSlashed = 0xE119,
    noteheadSquareBlackLarge = 0xE11A,
    noteheadSquareBlackWhite = 0xE11B,
    noteheadRoundBlackDoubleSlashed = 0xE11C,
    noteheadRoundWhiteDoubleSlashed = 0xE11D,

    augmentationDot = 0xE1E7,

    // tremolos
    tremolo1 = 0xE220,
    tremolo2 = 0xE221,
    tremolo3 = 0xE222,
    tremolo4 = 0xE223,
    tremolo5 = 0xE224,
    tremoloFingered1 = 0xE225,
    tremoloFingered2 = 0xE226,
    tremoloFingered3 = 0xE227,
    tremoloFingered4 = 0xE228,
    tremoloFingered5 = 0xE229,
    buzzRoll = 0xE22A,
    pendereckiTremolo = 0xE22B,
    unmeasuredTremolo = 0xE22C,
    unmeasuredTremoloSimple = 0xE22D,
    tremoloDivisiDots2 = 0xE22E,
    tremoloDivisiDots3 = 0xE22F,
    tremoloDivisiDots4 = 0xE230,
    tremoloDivisiDots6 = 0xE231,
    stockhausenTremolo = 0xE232,
    oneHandedRollStevens = 0xE233,
    doubleLateralRollStevens = 0xE234,

    flag8thUp = 0xE240,
    flag8thDown = 0xE241,
    flag16thUp = 0xE242,
    flag16thDown = 0xE243,
    flag32ndUp = 0xE244,
    flag32ndDown = 0xE245,
    flag64thUp = 0xE246,
    flag64thDown = 0xE247,
    flag128thUp = 0xE248,
    flag128thDown = 0xE249,
    flag256thUp = 0xE24A,
    flag256thDown = 0xE24B,
    flag512thUp = 0xE24C,
    flag512thDown = 0xE24D,
    flag1024thUp = 0xE24E,
    flag1024thDown = 0xE24F,
    flagInternalUp = 0xE250,
    flagInternalDown = 0xE251,

    accidentalFlat = 0xE260,
    accidentalNatural = 0xE261,
    accidentalSharp = 0xE262,
    accidentalDoubleSharp = 0xE263,
    accidentalDoubleFlat = 0xE264,
    accidentalTripleSharp = 0xE265,
    accidentalTripleFlat = 0xE266,
    accidentalNaturalFlat = 0xE267,
    accidentalNaturalSharp = 0xE268,
    accidentalSharpSharp = 0xE269,
    accidentalParensLeft = 0xE26A,
    accidentalParensRight = 0xE26B,
    accidentalBracketLeft = 0xE26C,
    accidentalBracketRight = 0xE26D,

    // articulations

    articAccentAbove = 0xE4A0,
    articAccentBelow = 0xE4A1,
    articStaccatoAbove = 0xE4A2,
    articStaccatoBelow = 0xE4A3,
    articTenutoAbove = 0xE4A4,
    articTenutoBelow = 0xE4A5,
    articStaccatissimoAbove = 0xE4A6,
    articStaccatissimoBelow = 0xE4A7,
    articStaccatissimoWedgeAbove = 0xE4A8,
    articStaccatissimoWedgeBelow = 0xE4A9,
    articStaccatissimoStrokeAbove = 0xE4AA,
    articStaccatissimoStrokeBelow = 0xE4AB,
    articMarcatoAbove = 0xE4AC,
    articMarcatoBelow = 0xE4AD,
    articMarcatoStaccatoAbove = 0xE4AE,
    articMarcatoStaccatoBelow = 0xE4AF,
    articAccentStaccatoAbove = 0xE4B0,
    articAccentStaccatoBelow = 0xE4B1,
    articTenutoStaccatoAbove = 0xE4B2,
    articTenutoStaccatoBelow = 0xE4B3,
    articTenutoAccentAbove = 0xE4B4,
    articTenutoAccentBelow = 0xE4B5,
    articStressAbove = 0xE4B6,
    articStressBelow = 0xE4B7,
    articUnstressAbove = 0xE4B8,
    articUnstressBelow = 0xE4B9,
    articLaissezVibrerAbove = 0xE4BA,
    articLaissezVibrerBelow = 0xE4BB,
    articMarcatoTenutoAbove = 0xE4BC,
    articMarcatoTenutoBelow = 0xE4BD,

    // holds and pauses

    fermataAbove = 0xE4C0,
    fermataBelow = 0xE4C1,
    fermataVeryShortAbove = 0xE4C2,
    fermataVeryShortBelow = 0xE4C3,
    fermataShortAbove = 0xE4C4,
    fermataShortBelow = 0xE4C5,
    fermataLongAbove = 0xE4C6,
    fermataLongBelow = 0xE4C7,
    fermataVeryLongAbove = 0xE4C8,
    fermataVeryLongBelow = 0xE4C9,
    fermataLongHenzeAbove = 0xE4CA,
    fermataLongHenzeBelow = 0xE4CB,
    fermataShortHenzeAbove = 0xE4CC,
    fermataShortHenzeBelow = 0xE4CD,
    breathMarkComma = 0xE4CE,
    breathMarkTick = 0xE4CF,
    breathMarkUpbow = 0xE4D0,
    caesura = 0xE4D1,
    caesuraThick = 0xE4D2,
    caesuraShort = 0xE4D3,
    caesuraCurved = 0xE4D4,
    breathMarkSalzedo = 0xE4D5,
    curlewSign = 0xE4D6,
    caesuraSingleStroke = 0xE4D7,

    // rests
    restMaxima = 0xE4E0,
    restLonga = 0xE4E1,
    restDoubleWhole = 0xE4E2,
    restWhole = 0xE4E3,
    restHalf = 0xE4E4,
    restQuarter = 0xE4E5,
    rest8th = 0xE4E6,
    rest16th = 0xE4E7,
    rest32nd = 0xE4E8,
    rest64th = 0xE4E9,
    rest128th = 0xE4EA,
    rest256th = 0xE4EB,
    rest512th = 0xE4EC,
    rest1024th = 0xE4ED,
    restHBar = 0xE4EE,
    restHBarLeft = 0xE4EF,
    restHBarMiddle = 0xE4F0,
    restHBarRight = 0xE4F1,
    restQuarterOld = 0xE4F2,
    restDoubleWholeLegerLine = 0xE4F3,
    restWholeLegerLine = 0xE4F4,
    restHalfLegerLine = 0xE4F5,
    restQuarterZ = 0xE4F6,

    // dynamics
    dynamicPiano = 0xE520,
    dynamicMezzo = 0xE521,
    dynamicForte = 0xE522,
    dynamicRinforzando = 0xE523,
    dynamicSforzando = 0xE524,
    dynamicZ = 0xE525,
    dynamicNiente = 0xE526,
    dynamicPPPPPP = 0xE527,
    dynamicPPPPP = 0xE528,
    dynamicPPPP = 0xE529,
    dynamicPPP = 0xE52A,
    dynamicPP = 0xE52B,
    dynamicMP = 0xE52C,
    dynamicMF = 0xE52D,
    dynamicPF = 0xE52E,
    dynamicFF = 0xE52F,
    dynamicFFF = 0xE530,
    dynamicFFFF = 0xE531,
    dynamicFFFFF = 0xE532,
    dynamicFFFFFF = 0xE533,
    dynamicFortePiano = 0xE534,
    dynamicForzando = 0xE535,
    dynamicSforzando1 = 0xE536,
    dynamicSforzandoPiano = 0xE537,
    dynamicSforzandoPianissimo = 0xE538,
    dynamicSforzato = 0xE539,
    dynamicSforzatoPiano = 0xE53A,
    dynamicSforzatoFF = 0xE53B,
    dynamicRinforzando1 = 0xE53C,
    dynamicRinforzando2 = 0xE53D,
    dynamicCrescendoHairpin = 0xE53E,
    dynamicDiminuendoHairpin = 0xE53F,
    dynamicMessaDiVoce = 0xE540,
    dynamicNienteForHairpin = 0xE541,
    dynamicHairpinParenthesisLeft = 0xE542,
    dynamicHairpinParenthesisRight = 0xE543,
    dynamicHairpinBracketLeft = 0xE544,
    dynamicHairpinBracketRight = 0xE545,
    dynamicCombinedSeparatorColon = 0xE546,
    dynamicCombinedSeparatorHyphen = 0xE547,
    dynamicCombinedSeparatorSpace = 0xE548,
    dynamicCombinedSeparatorSlash = 0xE549,

    // common ornaments

    graceNoteAcciaccaturaStemUp = 0xE560,
    graceNoteAcciaccaturaStemDown = 0xE561,
    graceNoteAppoggiaturaStemUp = 0xE562,
    graceNoteAppoggiaturaStemDown = 0xE563,
    graceNoteSlashStemUp = 0xE564,
    graceNoteSlashStemDown = 0xE565,
    ornamentTrill = 0xE566,
    ornamentTurn = 0xE567,
    ornamentTurnInverted = 0xE568,
    ornamentTurnSlash = 0xE569,
    ornamentTurnUp = 0xE56A,
    ornamentTurnUpS = 0xE56B,
    ornamentShortTrill = 0xE56C,
    ornamentMordent = 0xE56D,
    ornamentTremblement = 0xE56E,
    ornamentHaydn = 0xE56F,

    // string techniques

    stringsDownBow = 0xE610,
    stringsDownBowTurned = 0xE611,
    stringsUpBow = 0xE612,
    stringsUpBowTurned = 0xE613,
    stringsHarmonic = 0xE614,
    stringsHalfHarmonic = 0xE615,
    stringsMuteOn = 0xE616,
    stringsMuteOff = 0xE617,
    stringsBowBehindBridge = 0xE618,
    stringsBowOnBridge = 0xE619,
    stringsBowOnTailpiece = 0xE61A,
    stringsOverpressureDownBow = 0xE61B,
    stringsOverpressureUpBow = 0xE61C,
    stringsOverpressurePossibileDownBow = 0xE61D,
    stringsOverpressurePossibileUpBow = 0xE61E,
    stringsOverpressureNoDirection = 0xE61F,
    stringsJeteAbove = 0xE620,
    stringsJeteBelow = 0xE621,
    stringsFouette = 0xE622,
    stringsVibratoPulse = 0xE623,
    stringsThumbPosition = 0xE624,
    stringsThumbPositionTurned = 0xE625,
    stringsChangeBowDirection = 0xE626,
    stringsBowBehindBridgeOneString = 0xE627,
    stringsBowBehindBridgeTwoStrings = 0xE628,
    stringsBowBehindBridgeThreeStrings = 0xE629,
    stringsBowBehindBridgeFourStrings = 0xE62A,

    // guitar techniques
    guitarVibratoBarScoop = 0xE830,
    guitarVibratoBarDip = 0xE831,
    guitarShake = 0xE832,
    guitarString0 = 0xE833,
    guitarString1 = 0xE834,
    guitarString2 = 0xE835,
    guitarString3 = 0xE836,
    guitarString4 = 0xE837,
    guitarString5 = 0xE838,
    guitarString6 = 0xE839,
    guitarString7 = 0xE83A,
    guitarString8 = 0xE83B,
    guitarString9 = 0xE83C,
    guitarOpenPedal = 0xE83D,
    guitarHalfOpenPedal = 0xE83E,
    guitarClosePedal = 0xE83F,
    guitarLeftHandTapping = 0xE840,
    guitarRightHandTapping = 0xE841,
    guitarGolpe = 0xE842,
    guitarFadeIn = 0xE843,
    guitarFadeOut = 0xE844,
    guitarVolumeSwell = 0xE845,
    guitarStrumUp = 0xE846,
    guitarStrumDown = 0xE847,
    guitarBarreFull = 0xE848,
    guitarBarreHalf = 0xE849,
    guitarString10 = 0xE84A,
    guitarString11 = 0xE84B,
    guitarString12 = 0xE84C,
    guitarString13 = 0xE84D,

    // chord diagrams
    fretboardFilledCircle = 0xE858,
    fretboardX = 0xE859,
    fretboardO = 0xE85A,

    // tuplets
    tuplet0 = 0xE880,
    tuplet1 = 0xE881,
    tuplet2 = 0xE882,
    tuplet3 = 0xE883,
    tuplet4 = 0xE884,
    tuplet5 = 0xE885,
    tuplet6 = 0xE886,
    tuplet7 = 0xE887,
    tuplet8 = 0xE888,
    tuplet9 = 0xE889,
    tupletColon = 0xE88A,

    // multi-segment lines
    wiggleTrillFastest = 0xEAA0,
    wiggleTrillFasterStill = 0xEAA1,
    wiggleTrillFaster = 0xEAA2,
    wiggleTrillFast = 0xEAA3,
    wiggleTrill = 0xEAA4,
    wiggleTrillSlow = 0xEAA5,
    wiggleTrillSlower = 0xEAA6,
    wiggleTrillSlowerStill = 0xEAA7,
    wiggleTrillSlowest = 0xEAA8,
    wiggleArpeggiatoUp = 0xEAA9,
    wiggleArpeggiatoDown = 0xEAAA,
    wiggleArpeggiatoUpSwash = 0xEAAB,
    wiggleArpeggiatoDownSwash = 0xEAAC,
    wiggleArpeggiatoUpArrow = 0xEAAD,
    wiggleArpeggiatoDownArrow = 0xEAAE,
    wiggleGlissando = 0xEAAF,
    wiggleVibrato = 0xEAB0,
    wiggleVibratoWide = 0xEAB1,
    guitarVibratoStroke = 0xEAB2,
    guitarWideVibratoStroke = 0xEAB3,
    wiggleWavyNarrow = 0xEAB4,
    wiggleWavy = 0xEAB5,
    wiggleWavyWide = 0xEAB6,
    wiggleSquareWaveNarrow = 0xEAB7,
    wiggleSquareWave = 0xEAB8,
    wiggleSquareWaveWide = 0xEAB9,
    wiggleSawtoothNarrow = 0xEABA,
    wiggleSawtooth = 0xEABB,
    wiggleSawtoothWide = 0xEABC,
    wiggleGlissandoGroup1 = 0xEABD,
    wiggleGlissandoGroup2 = 0xEABE,
    wiggleGlissandoGroup3 = 0xEABF,
    wiggleCircularConstant = 0xEAC0,
    wiggleCircularConstantFlipped = 0xEAC1,
    wiggleCircularConstantLarge = 0xEAC2,
    wiggleCircularConstantFlippedLarge = 0xEAC3,
    wiggleCircularStart = 0xEAC4,
    wiggleCircularLargest = 0xEAC5,
    wiggleCircularLargerStill = 0xEAC6,
    wiggleCircularLarger = 0xEAC7,
    wiggleCircularLarge = 0xEAC8,
    wiggleCircular = 0xEAC9,
    wiggleCircularSmall = 0xEACA,
    wiggleCircularEnd = 0xEACB,
    wiggleVibratoStart = 0xEACC,
    wiggleVibratoSmallestFastest = 0xEACD,
    wiggleVibratoSmallestFasterStill = 0xEACE,
    wiggleVibratoSmallestFaster = 0xEACF,
    wiggleVibratoSmallestFast = 0xEAD0,
    wiggleVibratoSmallestSlow = 0xEAD1,
    wiggleVibratoSmallestSlower = 0xEAD2,
    wiggleVibratoSmallestSlowest = 0xEAD3,
    wiggleVibratoSmallFastest = 0xEAD4,
    wiggleVibratoSmallFasterStill = 0xEAD5,
    wiggleVibratoSmallFaster = 0xEAD6,
    wiggleVibratoSmallFast = 0xEAD7,
    wiggleVibratoSmallSlow = 0xEAD8,
    wiggleVibratoSmallSlower = 0xEAD9,
    wiggleVibratoSmallSlowest = 0xEADA,
    wiggleVibratoMediumFastest = 0xEADB,
    wiggleVibratoMediumFasterStill = 0xEADC,
    wiggleVibratoMediumFaster = 0xEADD,
    wiggleVibratoMediumFast = 0xEADE,
    wiggleVibratoMediumSlow = 0xEADF,
    wiggleVibratoMediumSlower = 0xEAE0,
    wiggleVibratoMediumSlowest = 0xEAE1,
    wiggleVibratoLargeFastest = 0xEAE2,
    wiggleVibratoLargeFasterStill = 0xEAE3,
    wiggleVibratoLargeFaster = 0xEAE4,
    wiggleVibratoLargeFast = 0xEAE5,
    wiggleVibratoLargeSlow = 0xEAE6,
    wiggleVibratoLargeSlower = 0xEAE7,
    wiggleVibratoLargeSlowest = 0xEAE8,
    wiggleVibratoLargestFastest = 0xEAE9,
    wiggleVibratoLargestFasterStill = 0xEAEA,
    wiggleVibratoLargestFaster = 0xEAEB,
    wiggleVibratoLargestFast = 0xEAEC,
    wiggleVibratoLargestSlow = 0xEAED,
    wiggleVibratoLargestSlower = 0xEAEE,
    wiggleVibratoLargestSlowest = 0xEAEF,
    wiggleRandom1 = 0xEAF0,
    wiggleRandom2 = 0xEAF1,
    wiggleRandom3 = 0xEAF2,
    wiggleRandom4 = 0xEAF3,
    beamAccelRit1 = 0xEAF4,
    beamAccelRit2 = 0xEAF5,
    beamAccelRit3 = 0xEAF6,
    beamAccelRit4 = 0xEAF7,
    beamAccelRit5 = 0xEAF8,
    beamAccelRit6 = 0xEAF9,
    beamAccelRit7 = 0xEAFA,
    beamAccelRit8 = 0xEAFB,
    beamAccelRit9 = 0xEAFC,
    beamAccelRit10 = 0xEAFD,
    beamAccelRit11 = 0xEAFE,
    beamAccelRit12 = 0xEAFF,
    beamAccelRit13 = 0xEB00,
    beamAccelRit14 = 0xEB01,
    beamAccelRit15 = 0xEB02,
    beamAccelRitFinal = 0xEB03,

    metNoteDoubleWhole = 0xECA0,
    metNoteDoubleWholeSquare = 0xECA1,
    metNoteWhole = 0xECA2,
    metNoteHalfUp = 0xECA3,
    metNoteHalfDown = 0xECA4,
    metNoteQuarterUp = 0xECA5,
    metNoteQuarterDown = 0xECA6,
    metNote8thUp = 0xECA7,
    metNote8thDown = 0xECA8,
    metNote16thUp = 0xECA9,
    metNote16thDown = 0xECAA,
    metNote32ndUp = 0xECAB,
    metNote32ndDown = 0xECAC,
    metNote64thUp = 0xECAD,
    metNote64thDown = 0xECAE,
    metNote128thUp = 0xECAF,
    metNote128thDown = 0xECB0,
    metNote256thUp = 0xECB1,
    metNote256thDown = 0xECB2,
    metNote512thUp = 0xECB3,
    metNote512thDown = 0xECB4,
    metNote1024thUp = 0xECB5,
    metNote1024thDown = 0xECB6,
    metAugmentationDot = 0xECB7
};

static SMuFLID GetNoteHeadSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return SMuFLID::None;
        case NoteValue::Whole: return SMuFLID::noteheadWhole;
        case NoteValue::Half: return SMuFLID::noteheadHalf;
        case NoteValue::Quarter: return SMuFLID::noteheadBlack;
        case NoteValue::Eighth: return SMuFLID::noteheadBlack;
        case NoteValue::Sixteenth: return SMuFLID::noteheadBlack;
        case NoteValue::ThirtySecond: return SMuFLID::noteheadBlack;
        default: return SMuFLID::noteheadDoubleWhole;
    }

    return SMuFLID::None;
}

static SMuFLID GetMetronomeNoteSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::Breve: return SMuFLID::metNoteDoubleWhole;
        case NoteValue::Whole: return SMuFLID::metNoteWhole;
        case NoteValue::Half: return SMuFLID::metNoteHalfUp;
        case NoteValue::Quarter: return SMuFLID::metNoteQuarterUp;
        case NoteValue::Eighth: return SMuFLID::metNote8thUp;
        case NoteValue::Sixteenth: return SMuFLID::metNote16thUp;
        case NoteValue::ThirtySecond: return SMuFLID::metNote32ndUp;
        case NoteValue::_64th: return SMuFLID::metNote64thUp;
        case NoteValue::_128th: return SMuFLID::metNote128thUp;
        case NoteValue::_256th: return SMuFLID::metNote256thUp;
        case NoteValue::_512th: return SMuFLID::metNote512thUp;
        case NoteValue::_1024th: return SMuFLID::metNote1024thUp;

        default: return SMuFLID::None;
    }
}

static SMuFLID GetRestSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return SMuFLID::None;
        case NoteValue::Whole: return SMuFLID::restWhole;
        case NoteValue::Half: return SMuFLID::restHalf;
        case NoteValue::Quarter: return SMuFLID::restQuarter;
        case NoteValue::Eighth: return SMuFLID::rest8th;
        case NoteValue::Sixteenth: return SMuFLID::rest16th;
        case NoteValue::ThirtySecond: return SMuFLID::rest32nd;
        default: return SMuFLID::restDoubleWhole;
    }

    return SMuFLID::None;
}

static SMuFLID GetNoteFlagSMuFLID(NoteValue type, bool isUp)
{
    switch (type)
    {
        case NoteValue::Eighth: if (isUp) return SMuFLID::flag8thUp; else return SMuFLID::flag8thDown;
        case NoteValue::Sixteenth: if (isUp) return SMuFLID::flag16thUp; else return SMuFLID::flag16thDown;
        case NoteValue::ThirtySecond: if (isUp) return SMuFLID::flag32ndUp; else return SMuFLID::flag32ndDown;
        default: return SMuFLID::None;
    }

    return SMuFLID::None;
}

static SMuFLID GetTimeSignatureSMuFLID(int time)
{
    switch (time)
    {
        case 0: return SMuFLID::timeSig0;
        case 1: return SMuFLID::timeSig1;
        case 2: return SMuFLID::timeSig2;
        case 3: return SMuFLID::timeSig3;
        case 4: return SMuFLID::timeSig4;
        case 5: return SMuFLID::timeSig5;
        case 6: return SMuFLID::timeSig6;
        case 7: return SMuFLID::timeSig7;
        case 8: return SMuFLID::timeSig8;
        case 9: return SMuFLID::timeSig9;
    }

    return SMuFLID::None;
}

#endif // MUSIQUE_SMUFLID_H