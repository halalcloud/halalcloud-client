/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SCRNCELL.H                                                            */
/*                                                                         */
/*   Defines the structs TCellChar and TScreenCell.                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_SCRNCELL_H
#define TVISION_SCRNCELL_H

#ifdef __BORLANDC__

inline const TColorAttr &getAttr(const TScreenCell &cell)
{
    return ((uchar *) &cell)[1];
}

inline void setAttr(TScreenCell &cell, TColorAttr attr)
{
    ((uchar *) &cell)[1] = attr;
}

inline const TCellChar &getChar(const TScreenCell &cell)
{
    return ((uchar *) &cell)[0];
}

inline void setChar(TScreenCell &cell, TCellChar ch)
{
    ((uchar *) &cell)[0] = ch;
}

inline void setCell(TScreenCell &cell, TCellChar ch, TColorAttr attr)
{
    cell = ushort((attr << 8) | ch);
}

#else

//// TCellChar
//
// Represents text in a screen cell. You should usually not need to interact
// with this manually. In order to write text into a screen cell, just use
// the functions in the TText namespace.
//
// INVARIANT:
// * '_text' contains one of the following:
//     1. A single byte of ASCII or 'extended ASCII' text (1 column wide).
//     2. Up to 15 bytes of UTF-8 text (1 or 2 columns wide in total,
//        meaning that it must not contain just zero-width characters).
//     3. A special value that marks it as wide char trail.

struct TCellChar
{
    enum : uint8_t { fWide = 0x1, fTrail = 0x2 };

    char _text[15];
    uint8_t
        _textLength : 4,
        _flags : 4;

    TCellChar() = default;
    inline void moveChar(char ch);
    inline void moveMultiByteChar(uint32_t mbc, bool wide = false);
    inline void moveMultiByteChar(TStringView mbc, bool wide = false);
    inline void moveWideCharTrail();

    constexpr inline bool isWide() const;
    constexpr inline bool isWideCharTrail() const;
    constexpr inline void appendZeroWidthChar(TStringView mbc);
    constexpr inline TStringView getText() const;
    constexpr inline size_t size() const;

    constexpr inline char& operator[](size_t i);
    constexpr inline const char& operator[](size_t i) const;
};

inline void TCellChar::moveChar(char ch)
{
    memset(this, 0, sizeof(*this));
    _text[0] = ch;
    _textLength = 1;
}

inline void TCellChar::moveMultiByteChar(uint32_t mbc, bool wide)
// Pre: 'mbc' is a bit-casted multibyte-encoded character.
{
    memset(this, 0, sizeof(*this));
    memcpy(_text, &mbc, sizeof(mbc));
    _flags = -int(wide) & fWide;
#ifndef TV_BIG_ENDIAN
    _textLength = 1 + ((mbc & 0xFF00) != 0) +
                      ((mbc & 0xFF0000) != 0) +
                      ((mbc & 0xFF000000) != 0);
#else
    _textLength = 1 + ((mbc & 0xFF0000) != 0) +
                      ((mbc & 0xFF00) != 0) +
                      ((mbc & 0xFF) != 0);
#endif
}

inline void TCellChar::moveMultiByteChar(TStringView mbc, bool wide)
{
    static_assert(sizeof(_text) >= maxCharSize, "");
    memset(this, 0, sizeof(*this));
    if (0 < mbc.size() && mbc.size() <= maxCharSize)
    {
        _flags |= -int(wide) & fWide;
        switch (mbc.size())
        {
            case 4: _text[3] = mbc[3];
            case 3: _text[2] = mbc[2];
            case 2: _text[1] = mbc[1];
            case 1: _text[0] = mbc[0];
        }
        _textLength = mbc.size();
    }
}

inline void TCellChar::moveWideCharTrail()
{
    memset(this, 0, sizeof(*this));
    _flags = fTrail;
}

constexpr inline bool TCellChar::isWide() const
{
    return _flags & fWide;
}

constexpr inline bool TCellChar::isWideCharTrail() const
{
    return _flags & fTrail;
}

constexpr inline void TCellChar::appendZeroWidthChar(TStringView mbc)
// Pre: !isWideCharTrail();
{
    size_t sz = size();
    if (mbc.size() <= sizeof(_text) - sz)
    {
        if (_text[0] == '\0')
            _text[0] = ' ';
        switch (mbc.size())
        {
            case 4: _text[sz + 3] = mbc[3];
            case 3: _text[sz + 2] = mbc[2];
            case 2: _text[sz + 1] = mbc[1];
            case 1: _text[sz] = mbc[0];
        }
        _textLength = sz + mbc.size();
    }
}

constexpr inline TStringView TCellChar::getText() const
{
    return {_text, size()};
}

constexpr inline size_t TCellChar::size() const
{
    // There is always at least one character, even if it is a null byte and
    // '_textLength' is zero (e.g. because the TCellChar was zero-initialized).
    return max(_textLength, 1);
}

constexpr inline char& TCellChar::operator[](size_t i)
{
    return _text[i];
}

constexpr inline const char& TCellChar::operator[](size_t i) const
{
    return _text[i];
}

//// TScreenCell
//
// Stores the text and color attributes in a screen cell.
// Please use the functions in the TText namespace in order to fill screen cells
// with text.
//
// Considerations:
// * In order for a double-width character to be displayed entirely, its cell
//   must be followed by another containing a wide char trail. If it is not,
//   or if a wide char trail is not preceded by a double-width character,
//   we'll understand that a double-width character is being overlapped partially.

struct TScreenCell
{
    TColorAttr attr;
    TCellChar _ch;

    TScreenCell() = default;
    inline TScreenCell(ushort bios);
    TV_TRIVIALLY_ASSIGNABLE(TScreenCell)

    constexpr inline bool isWide() const;

    inline bool operator==(const TScreenCell &other) const;
    inline bool operator!=(const TScreenCell &other) const;
};

inline const TColorAttr &getAttr(const TScreenCell &cell);
inline void setAttr(TScreenCell &cell, const TColorAttr &attr);
inline void setChar(TScreenCell &cell, char ch);
inline void setCell(TScreenCell &cell, char ch, const TColorAttr &attr);

inline TScreenCell::TScreenCell(ushort bios)
{
    memset(this, 0, sizeof(*this));
    _ch.moveChar(char(bios));
    attr = uchar(bios >> 8);
}

constexpr inline bool TScreenCell::isWide() const
{
    return _ch.isWide();
}

inline bool TScreenCell::operator==(const TScreenCell &other) const
{
    return memcmp(this, &other, sizeof(*this)) == 0;
}

inline bool TScreenCell::operator!=(const TScreenCell &other) const
{
    return !(*this == other);
}

inline const TColorAttr &getAttr(const TScreenCell &cell)
{
    return cell.attr;
}

inline void setAttr(TScreenCell &cell, const TColorAttr &attr)
{
    cell.attr = attr;
}

inline void setChar(TScreenCell &cell, char ch)
{
    cell._ch.moveChar(ch);
}

inline void setCell(TScreenCell &cell, char ch, const TColorAttr &attr)
{
    memset(&cell, 0, sizeof(cell));
    ::setChar(cell, ch);
    ::setAttr(cell, attr);
}

#endif // __BORLANDC__

#endif // TVISION_SCRNCELL_H
