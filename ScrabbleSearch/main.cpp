#include "XY.h"
#include "trace.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <functional>

using namespace std;

class DAWG
{
public:
   struct Node
   {
      bool isLeaf = false;
      uint32_t children[26] = { 0 };

      bool operator==( const Node& rhs ) const
      {
         if ( isLeaf != rhs.isLeaf )
            return false;
         for ( int i = 0; i < 26; i++ )
            if ( children[i] != rhs.children[i] )
               return false;
         return true;
      }
      string str() const 
      {
         stringstream ss;
         ss << (isLeaf ? "L" : ".");
         for ( int i = 0; i < 26; i++ )
            if ( children[i] )
               ss << " " << char( i + 'A' ) << children[i];
         return ss.str();
      }
   }; 
   struct NodeHasher
   {
      size_t operator()( const Node& node ) const
      {
         size_t ret = node.isLeaf ? 1 : 0;
         for ( uint64_t e : node.children )
            ret ^= e + 0x9e3779b9 + (ret << 6) + (ret >> 2);
         return ret;
      }
   };

   bool isValidWord( const string& str ) const
   {
      int idx = 0;
      for ( char c : str )
      {
         idx = m_nodes[idx].children[c-'A'];
         if ( idx == 0 )
            return false;
      }
      return m_nodes[idx].isLeaf;
   }

public:
   DAWG()
   {
   }
   void addWord( const string& s )
   {
      if ( m_nodes.empty() )
         m_nodes.push_back( Node() );
      addWord( s, 0, m_nodes[0] );
   }

   //int count() const { return count( "", m_nodes[0] ); }
   int count() const 
   { 
      int ret = 0;
      forEachWord( [&]( const string& s ) { ret++; } );
      return ret;
   }

   DAWG compressed() const
   {
      DAWG ret = *this;
      while ( true )
      {         
         trace << "m_nodes.size() = " << ret.m_nodes.size() << endl;
         DAWG compressedRet = ret.compressedOnce();
         if ( ret.m_nodes.size() == compressedRet.m_nodes.size() )
            return ret;
         ret = compressedRet;
      }
   }
   DAWG compressedOnce() const
   {
      DAWG ret;
      unordered_map<Node, uint32_t, NodeHasher> uniqueNodes;

      vector<uint32_t> newIndex( m_nodes.size() );
      vector<bool> isIndexUsed( m_nodes.size() );

      uint32_t j = 0;
      for ( int i = 0; i < (int)m_nodes.size(); i++ )
         if ( uniqueNodes.insert( make_pair( m_nodes[i], j ) ).second )
         {
            newIndex[i] = j++;
            isIndexUsed[i] = true;
         }
         else
            newIndex[i] = uniqueNodes.at( m_nodes[i] );

      for ( int i = 0; i < (int)m_nodes.size(); i++ ) if ( isIndexUsed[i] )
      {
         Node n = m_nodes[i];
         for ( uint32_t& child : n.children )
            child = newIndex[child];
         ret.m_nodes.push_back( n );
      }   

      return ret;
   }

   void saveToDisk( const string& filename ) const
   {
      ofstream f( filename, ios::binary );
      uint32_t nodeCount = (uint32_t) m_nodes.size();
      f.write( (const char*)&nodeCount, 4 );
      for ( int i = 0; i < (int) m_nodes.size(); i++ )
         f.write( (const char*)&m_nodes[i], sizeof(Node) );
   }

   static DAWG fromDisk( const string& filename )
   {
      ifstream f( filename, ios::binary );
      uint32_t nodeCount = 0;
      f.read( (char*)&nodeCount, 4 );

      DAWG ret;
      for ( uint32_t i = 0; i < nodeCount; i++ )
      {
         Node node;
         f.read( (char*)&node, sizeof( Node ) );
         ret.m_nodes.push_back( node );
      }
      return ret;
   }

   string str() const
   {
      stringstream ss;
      for ( int i = 0; i < (int) m_nodes.size(); i++ )
         ss << i << "\t" << m_nodes[i].str() << endl;
      return ss.str();
   }

   void forEachWord( const function<void(const string&)>& f ) const
   {
      forEachWord( "", m_nodes[0], f );
   }

private:
   void addWord( const string& s, int charIndex, Node& node )
   {
      if ( charIndex == s.length() )
      {
         node.isLeaf = true;
         return;
      }

      uint32_t childNodeIndex = node.children[s[charIndex] - 'A'];
      if ( childNodeIndex == 0 )
      {
         childNodeIndex = (uint32_t)m_nodes.size();
         node.children[s[charIndex] - 'A'] = childNodeIndex;
         m_nodes.push_back( Node() );
      }
      addWord( s, charIndex + 1, m_nodes[childNodeIndex] );
   }

   void forEachWord( const string& s, const Node& node, const function<void( const string& )>& f ) const
   {
      if ( node.isLeaf ) 
         f( s );
      for ( char ch = 'A'; ch <= 'Z'; ch++ )
         if ( node.children[ch - 'A'] )
            forEachWord( s + ch, m_nodes[node.children[ch - 'A']], f );
   }

   //int count( const string& s, const Node& node ) const
   //{
   //   int ret = 0;
   //   if ( node.isLeaf ) ret++;
   //   for ( char ch = 'A'; ch <= 'Z'; ch++ )
   //      if ( node.children[ch - 'A'] )
   //         ret += count( s + ch, m_nodes[node.children[ch - 'A']] );
   //   return ret;
   //}

public:
   vector<Node> m_nodes;
};

DAWG createDAWG()
{
   string name = "csw19";

   if ( filesystem::exists( name + ".dawg" ) )
      return DAWG::fromDisk( name + ".dawg" );

   DAWG dawg;
   ifstream f( name + ".txt" );
   string word;
   int ct = 0;
   while ( f >> word )
      dawg.addWord( word );

   for ( char c = 'A'; c <= 'Z'; c++ )
      dawg.addWord( string() + c );

   trace << "original node count = " << dawg.count() << endl;
   dawg = dawg.compressed();
   trace << "compressed node count = " << dawg.count() << endl;

   dawg.saveToDisk( name + ".dawg" );
   return dawg;
}

namespace
{
   int g_tileValues[26] = { 1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10 };
   int faceValueForTile( char c ) { return c >= 'A' && c <= 'Z' ? g_tileValues[c - 'A'] : 0; }
   int faceValueForTiles( const std::string& s ) { int ret = 0;  for ( char c : s ) ret += faceValueForTile( c ); return ret; }
}

class BonusSquares
{
public:
   struct Bonus
   {
      int wordMultiplier = 1;
      int letterMultiplier = 1;
      char toChar() const { static string s = R"( '"*..$..)"; return s[wordMultiplier * 3 + letterMultiplier - 4]; }
   };
   BonusSquares()
   {
      m_bonuses = std::vector<std::vector<Bonus>>( m_size.y, std::vector<Bonus>( m_size.x ) );
      static vector<XY> bonus_3_1 = { XY( 0, 0 ), XY( 7, 0 ) };
      static vector<XY> bonus_2_1 = { XY( 1, 1 ), XY( 2, 2 ), XY( 3, 3 ), XY( 4, 4 ), XY( 7, 7 ) };
      static vector<XY> bonus_1_3 = { XY( 5, 1 ), XY( 5, 5 ) };
      static vector<XY> bonus_1_2 = { XY( 3, 0 ), XY( 6, 2 ), XY( 6, 6 ), XY( 7, 3 ) };
      auto sym = []( XY p ) { return vector<XY>{ p, XY( 14 - p.x, p.y ), XY( p.x, 14 - p.y ), XY( 14 - p.x, 14 - p.y ), XY( p.y, p.x ), XY( 14 - p.y, p.x ), XY( p.y, 14 - p.x ), XY( 14 - p.y, 14 - p.x ) }; };
      for ( const XY& p : bonus_3_1 ) for ( XY q : sym( p ) ) m_bonuses[q.y][q.x] = Bonus{ 3, 1 };
      for ( const XY& p : bonus_2_1 ) for ( XY q : sym( p ) ) m_bonuses[q.y][q.x] = Bonus{ 2, 1 };
      for ( const XY& p : bonus_1_3 ) for ( XY q : sym( p ) ) m_bonuses[q.y][q.x] = Bonus{ 1, 3 };
      for ( const XY& p : bonus_1_2 ) for ( XY q : sym( p ) ) m_bonuses[q.y][q.x] = Bonus{ 1, 2 };
   }
   const Bonus& operator[]( const XY& p ) const
   {
      return m_bonuses[p.y][p.x];
   }
   string str() const
   {
      stringstream ss;
      for ( XY p : m_size )
      {
         if ( p.x == 0 && p.y != 0 )
            ss << endl;
         ss << (*this)[p].toChar();
      }
      return ss.str();
   }
public:
   XY m_size = XY( 15, 15 );
   std::vector<std::vector<Bonus>> m_bonuses;
};

class Board
{
public:
   struct Square
   {
      char c = 0;
      bool isEmpty() const { return c == 0; }
      char toChar() const { return isEmpty() ? ' ' : c; }
   };

public:
   Board()
   {
      m_board = std::vector<std::vector<Square>>( m_size.y, std::vector<Square>( m_size.x ) );
   }   

   Square operator[]( const XY& p ) const
   {
      return m_board[p.y][p.x];
   }

   bool isValidPos( const XY& p ) const { return p.x >= 0 && p.y >= 0 && p.x < m_size.x && p.y < m_size.y; }

   void setAt( const XY& p, char c )
   {
      m_board[p.y][p.x] = Square{ c };
   }

   XY size() const { return m_size; }

   void set( const std::string& s )
   {
      for ( XY p : m_size )
         setAt( p, 0 );
      XY p = XY( 0, -1 );
      for ( char c : s )
      {
         if ( c == '\n' ) { p = XY( 0, p.y + 1 ); continue; }
         if ( !(c == '.' || isalpha( c )) )
            throw std::runtime_error( "Board::set(): invalid char" );

         if ( c != '.' )
            setAt( p, c );
         p += XY( 1, 0 );
      }
   }

   string str() const
   {
      BonusSquares bonus;

      stringstream ss;
      for ( XY p : m_size )
      {
         if ( p.x == 0 && p.y != 0 )
            ss << endl;

         char c = bonus[p].toChar();
         if ( !(*this)[p].isEmpty() )
            c = (*this)[p].toChar();
         ss << c;
      }
      return ss.str();
   }

   bool isTileAt( const XY& p ) const { return isValidPos( p ) && !(*this)[p].isEmpty(); }

   string tilesAbove( XY p ) const
   {
      string ret;
      for ( p.y--; isTileAt( p ); p.y-- )
         ret += (*this)[p].c;
      std::reverse( ret.begin(), ret.end() );
      return ret;
   }
   string tilesBelow( XY p ) const
   {
      string ret;
      for ( p.y++; isTileAt( p ); p.y++ )
         ret += (*this)[p].c;
      return ret;
   }
   Board flippedXY() const
   {
      Board ret;
      ret.m_size = XY( m_size.y, m_size.x );
      ret.m_board = std::vector<std::vector<Square>>( ret.m_size.y, std::vector<Square>( ret.m_size.x ) );
      for ( XY p : m_size )
         ret.m_board[p.x][p.y] = (*this)[p];
      return ret;
   }

public:
   XY m_size = XY( 15, 15 );
   std::vector<std::vector<Square>> m_board;
};

struct Play
{
   XY pos;
   bool horizontal = true;
   int score = 0;
   std::string text;

   Play flippedXY() const
   {
      Play ret = *this;
      ret.horizontal = !ret.horizontal;
      ret.pos = XY( pos.y, pos.x );
      return ret;
   }

   string str( const Board* board = nullptr ) const
   {
      string ret = horizontal ? to_string( 1 + pos.y ) + char( 'A' + pos.x )
         : char( 'A' + pos.x ) + to_string( 1 + pos.y );
      stringstream ss;
      ss << setw( 3 ) << score << " " << setw( 3 ) << ret << " " << ( board ? textWithParens( *board ) : text);
      return ss.str();
   }

   string textWithParens( const Board& board ) const
   {
      XY p = pos;
      XY dir = horizontal ? XY( 1, 0 ) : XY( 0, 1 );
      string ret;
      for ( int i = 0; i < text.length(); i++ )
      {
         if ( !board.isTileAt( p + dir * (i - 1) ) && board.isTileAt( p + dir * i ) )
            ret += '(';
         ret += text[i];
         if ( board.isTileAt( p + dir * i ) && !board.isTileAt( p + dir * (i + 1) ) )
            ret += ')';
      }
      return ret;
   }
};

class HorizontalMoveFinder
{
public:
   struct Square
   {
      char c = 0;
      bool isAnchor = false;
      uint32_t validLetters = (1 << 26) - 1;
      int points = 0;
      BonusSquares::Bonus bonus;

      bool isEmpty() const { return c == 0; }
      bool isValidLetter( char c ) const { return c == '?' ? validLetters != 0 : (( validLetters >> (c - 'A') ) & 1) != 0; }
      static Square occupiedSquare( char c ) { Square ret; ret.c = c; return ret; }
      static Square emptySquare( const BonusSquares::Bonus& bonus ) { Square ret; ret.bonus = bonus; return ret; }
   };

public:
   HorizontalMoveFinder( const DAWG& dawg, const Board& board, const BonusSquares& bonus, const string& rack, function<void(const Play&)> onPlayFoundFunc )
      : m_dawg( dawg )
      , m_board( board )
      , m_bonus( bonus )
      , m_onPlayFoundFunc( onPlayFoundFunc )
   {
      m_squares = std::vector<std::vector<Square>>( m_board.size().y, std::vector<Square>(m_board.size().x));

      for ( XY p : m_board.size() )
         m_squares[p.y][p.x] = calcSquareAt( p );

      for ( XY p : m_board.size() ) if ( !m_board.isTileAt( p - XY(1,0) ) )
         findMoves( p, rack );
   }
   Square calcSquareAt( const XY& p ) const
   {
      if ( !m_board[p].isEmpty() )
         return Square::occupiedSquare( m_board[p].c );

      string prefix = m_board.tilesAbove( p );
      string suffix = m_board.tilesBelow( p );

      if ( prefix.empty() && suffix.empty() )
         return Square::emptySquare( m_bonus[p] );

      uint32_t validLetters = 0;
      for ( char c = 'A'; c <= 'Z'; c++ )
         if ( m_dawg.isValidWord( prefix + c + suffix ) )
            validLetters |= (1 << (c - 'A'));

      BonusSquares::Bonus bonus = m_bonus[p];
      int points = bonus.wordMultiplier * faceValueForTiles( prefix + suffix );
      return Square{ 0, true, validLetters, points, bonus };
   }
   const Square& at( const XY& p ) const
   {
      return m_squares[p.y][p.x];
   }
   void findMoves( const XY& pos, const string& rack_ ) const
   {
      string rack = rack_;
      sort( rack.begin(), rack.end() );

      int minEndX;
      if ( m_board[pos].isEmpty() ) // `minEndX` makes sure that the word is long enough to connect to existing tile
      {
         int tilesLeftOnRack = (int)rack.length();
         for ( minEndX = pos.x; minEndX < m_board.size().x; minEndX++ )
         {
            if ( at( XY( minEndX, pos.y ) ).isAnchor || !at( XY( minEndX, pos.y ) ).isEmpty() )
               break;
            if ( --tilesLeftOnRack <= 0 )
               return;
         }
         if ( minEndX == m_board.size().x )
            return;
      }
      else // `minEndX` makes sure that at least one tile is played
      {
         for ( minEndX = pos.x; !m_board[XY(minEndX, pos.y)].isEmpty(); minEndX++ );
      }

      string str = "";
      findMoves( pos, rack, m_dawg.m_nodes[0], minEndX, str, 0, 0, 1 );
   }
   void findMoves( const XY& pos, string& rack, const DAWG::Node& dawgNode, int minEndX, std::string& str, int verticalScore, int horizontalScore, int horizontalMultiplier ) const
   {
      if ( m_board.isTileAt( pos ) )
      {
         int tileVal = faceValueForTile( m_board[pos].c );
         int childNodeIndex = dawgNode.children[toupper( m_board[pos].c ) - 'A'];
         if ( childNodeIndex == 0 )
            return;
         const DAWG::Node& dawgChildNode = m_dawg.m_nodes[childNodeIndex];
         str.push_back( m_board[pos].c );
         findMoves( pos + XY( 1, 0 ), rack, dawgChildNode, minEndX, str, verticalScore, horizontalScore + tileVal, horizontalMultiplier );
         str.pop_back();
         return;
      }

      if ( dawgNode.isLeaf && pos.x > minEndX )
      {
         if ( std::count( rack.begin(), rack.end(), '.' ) == 0 )
            return; // no tiles played
         int score = verticalScore + horizontalScore * horizontalMultiplier;
         if ( rack == "......." ) 
            score += 50;
         //trace << str << " " << score << endl;
         m_onPlayFoundFunc( Play{ pos - XY( str.length(), 0 ), true /*horizontal*/, score, str});
      }

      if ( pos.x >= m_board.size().x )
         return;

      char prevC = 0;
      for ( char& c : rack ) if ( c != '.' )
      {
         if ( c <= prevC )
            continue; // don't consider same tile twice
         prevC = c;
         const Square& square = at( pos );

         bool isBlank = c == '?';
         char cMin = isBlank ? 'A' : c;
         char cMax = isBlank ? 'Z' : c;
         for ( char cc = cMin; cc <= cMax; cc++ ) if ( at( pos ).isValidLetter( cc ) )
         {
            uint32_t childIndex = dawgNode.children[cc - 'A'];
            if ( childIndex == 0 )
               continue;
            const DAWG::Node& dawgChildNode = m_dawg.m_nodes[childIndex];
            char origC = '.';
            std::swap( origC, c );

            str.push_back( isBlank ? tolower( cc ) : cc );
            int tileVal = faceValueForTile( origC ) * square.bonus.letterMultiplier;
            findMoves( pos + XY( 1, 0 ), rack, dawgChildNode, minEndX, str, verticalScore + square.points + (square.isAnchor ? tileVal * square.bonus.wordMultiplier : 0), horizontalScore + tileVal, horizontalMultiplier * square.bonus.wordMultiplier );
            str.pop_back();

            std::swap( origC, c );
         }
      }
   }

public:
   const DAWG& m_dawg;
   Board m_board;
   BonusSquares m_bonus;
   std::vector<std::vector<Square>> m_squares;
   function<void( const Play& )> m_onPlayFoundFunc;
};

void findMoves( const DAWG& dawg, const Board& board, const BonusSquares& bonus, const string& rack, function<void( const Play& )> onPlayFoundFunc )
{
   HorizontalMoveFinder( dawg, board, BonusSquares(), rack, onPlayFoundFunc );
   HorizontalMoveFinder( dawg, board.flippedXY(), BonusSquares(), rack, [&]( const Play& play )
   {
      onPlayFoundFunc( play.flippedXY() );
   } );
}

int calcSUS( const DAWG& dawg, const Board& board, const BonusSquares& bonus, const string& rack )
{
   vector<Play> plays;
   auto onPlayFoundFunc = [&]( const Play& play ) { plays.push_back( play ); };
   findMoves( dawg, board, bonus, rack, onPlayFoundFunc );

   stable_sort( plays.begin(), plays.end(), []( const Play& a, const Play& b ) { return a.score < b.score; } );

   vector<int> scoreHistogram( 1000 );
   for ( const Play& play : plays )
      scoreHistogram[play.score]++;

   int sus;
   for ( sus = 1; scoreHistogram[sus] > 0; sus++ );

   int curScore = 0;
   for ( const Play& play : plays ) if ( play.score > curScore )
   {
      curScore = play.score;
      trace << "[x" << setw( 4 ) << scoreHistogram[play.score] << "]" << play.str(&board) << endl;
   }
   return sus;
}

int calcNumBigScores( const DAWG& dawg, const Board& board, const BonusSquares& bonus, const string& rack, string& info )
{
   vector<Play> plays;
   auto onPlayFoundFunc = [&]( const Play& play ) { plays.push_back( play ); };
   findMoves( dawg, board, bonus, rack, onPlayFoundFunc );

   int numBigScores = 0;

   int MIN_SCORE = 100;
   vector<int> scoreHistogram( 1000 );
   for ( const Play& play : plays )
      if ( scoreHistogram[play.score]++ == 0 && play.score >= 100 )
      {
         int infoIdx = play.score - MIN_SCORE;
         if ( infoIdx+1 > info.length() )
            info.resize( infoIdx + 1, ' ' );
         info[infoIdx] = '#';
         numBigScores++;
      }

   return numBigScores;
}

void forEachRack( const string& rack, char startLetter, const function<void(const string&)> f )
{
   if ( rack.length() == 7 )
      return f( rack );
   for ( char c = startLetter; c <= 'Z'; c++ )
      forEachRack( rack + c, c, f );
}
void forEachRack( const string& rack, const function<void( const string& )> f )
{
   forEachRack( rack, 'A', f );
}

void go( const DAWG& dawg, const string& rack )
{
   int bestSUS = 0;
   dawg.forEachWord( [&]( const string& startWord ) 
   {
      if ( startWord.length() > 7 )
         return;

      string letters = rack + startWord;
      if ( count( letters.begin(), letters.end(), 'X' ) > 1 ) return;
      if ( count( letters.begin(), letters.end(), 'Z' ) > 1 ) return;
      if ( count( letters.begin(), letters.end(), 'J' ) > 1 ) return;
      if ( count( letters.begin(), letters.end(), 'Q' ) > 1 ) return;
      if ( count( letters.begin(), letters.end(), 'M' ) > 2 ) return;
      
      for ( int x = 8 - startWord.length(); x <= 7; x++ )
      {
         if ( x > 6 || x + startWord.length() <= 8 ) continue;
         if ( string( "HMEAOIU" ).find( startWord[6 - x] ) == string::npos ) continue;
         if ( string( "HMEAOIU" ).find( startWord[8 - x] ) == string::npos ) continue;

         Board board;
         for ( int i = 0; i < startWord.length(); i++ )
            board.m_board[7][x + i].c = startWord[i];

         int sus = calcSUS( dawg, board, BonusSquares(), rack );
         if ( sus + 5 >= bestSUS )
            cout << sus << " " << startWord << " " << x << endl;

         if ( sus >= bestSUS )
         {
            bestSUS = sus;
            trace << rack << " " << sus << " " << startWord << " " << x << endl;
         }
      }
   
   } );
}

#pragma warning(disable:4326) // void main
void main()
{
   DAWG dawg = createDAWG();
   trace << "dawg.count() = " << dawg.count() << endl;

   ////for ( string rack : { "??MAZED", "??ZEDMO", "??AADMZ", "??AEFMZ", "??AEHMZ", "?ACDEMZ", "?ADEHMZ", "?ADEMSZ", "?ADEMYZ", "?AEHNZZ" } )   
   ////for ( string rack : { "??MZAEY" } )   
   ////for ( string rack : { "??MZAES", "??HZAES", "??YZAES", "??KZAES", "??FZAES", "??HZAED", "??YZAED", "??YZAEH" } )
   //for ( string rack : { "??MAXED", "??XEDMO", "??AADMX", "??AEFMX", "??AEHMX", "?ACDEMX", "?ADEHMX", "?ADEMSX", "?ADEMYX", "??MXAEY", "??MXAES", "??HXAES", "??YXAES", "??KXAES", "??FXAES", "??HXAED", "??YXAED", "??YXAEH" } )
   //{
   //   trace << "RACK = " << rack << endl;
   //   go( dawg, rack );
   //}
   //return;

   Board board;
   board.set( R"(
............E..
..........DEW..
............E..
.....JUXTAPOSE.
..........E..A.
.......Q..R....
.......U..I..A.
.PSYCHOANALYZE.
.......D...I...
.......R...NO..
.......I....HO.
.......L....MO.
.......L.......
.....AREA......
...............
)" );

   trace << board.str() << endl;

   int sus = calcSUS( dawg, board, BonusSquares(), "??ADEMS" );
   trace << "sus = " << sus << endl;
   //string info;
   //int numBigScores = calcNumBigScores( dawg, board, BonusSquares(), "??ADEMS", info );
   //trace << "numBigScores = " << numBigScores << " " << info << endl;
   return;


   //int best = 0;
   //string curPrefix;
   //forEachRack( "?", [&]( const string& rack ) {
   //   if ( rack.find_first_of( "BFQWVJ" ) == string::npos )
   //      return;
   //   int sus = calcSUS( dawg, board, BonusSquares(), rack );
   //   string prefix = rack.substr( 0, 2 );
   //   if ( prefix != curPrefix )
   //      cout << rack << " sus = " << sus << endl;
   //   curPrefix = prefix;
   //   if ( sus >= best )
   //   {
   //      trace << rack << " sus = " << sus << (sus >= best ? " *** " : "") << endl;
   //      best = sus;
   //   }
   //} );
   

   
   int best = 0;
   string curPrefix;
   forEachRack( "??", [&]( const string& rack ) {
      string info;
      int numBigScores = calcNumBigScores( dawg, board, BonusSquares(), rack, info );
      string prefix = rack.substr( 0, 2 );
      if ( prefix != curPrefix )
         cout << rack << " sus = " << numBigScores << endl;
      curPrefix = prefix;
      if ( numBigScores >= best )
      {
         trace << rack << " numBigScores = " << numBigScores << (numBigScores >= best ? " *** " : "") << " " << info << endl;
         best = numBigScores;
      }
   } );


}
