#include "Alignment/MuonAlignment/interface/AlignableMuBarrel.h"


/// The constructor simply copies the vector of wheels and computes the surface from them
AlignableMuBarrel::AlignableMuBarrel( const std::vector<AlignableDTWheel*> dtWheels ) 
{

  theDTWheels.insert( theDTWheels.end(), dtWheels.begin(), dtWheels.end() );

  setSurface( computeSurface() );
   
}
      

/// Clean delete of the vector and its elements
AlignableMuBarrel::~AlignableMuBarrel() 
{
  for ( std::vector<AlignableDTWheel*>::iterator iter = theDTWheels.begin(); 
	iter != theDTWheels.end(); iter++)
    delete *iter;

}

/// Return AlignableBarrelLayer at given index
AlignableDTWheel &AlignableMuBarrel::wheel(int i) 
{
  
  if (i >= size() ) 
	throw cms::Exception("LogicError") << "Wheel index (" << i << ") out of range";

  return *theDTWheels[i];
  
}


/// Returns surface corresponding to current position
/// and orientation, as given by average on all components
AlignableSurface AlignableMuBarrel::computeSurface()
{

  return AlignableSurface( computePosition(), computeOrientation() );

}



/// Compute average z position from all components (x and y forced to 0)
AlignableMuBarrel::PositionType AlignableMuBarrel::computePosition() 
{

  float zz = 0.;

  for ( std::vector<AlignableDTWheel*>::iterator ilayer = theDTWheels.begin();
		ilayer != theDTWheels.end(); ilayer++ )
    zz += (*ilayer)->globalPosition().z();

  zz /= static_cast<float>(theDTWheels.size());

  return PositionType( 0.0, 0.0, zz );

}


/// Just initialize to default given by default constructor of a RotationType
AlignableMuBarrel::RotationType AlignableMuBarrel::computeOrientation() 
{
  return RotationType();
}


/// Twists all components by given angle
void AlignableMuBarrel::twist(float rad) 
{

  for ( std::vector<AlignableDTWheel*>::iterator iter = theDTWheels.begin();
	   iter != theDTWheels.end(); iter++ ) 
	(*iter)->twist(rad);
  
}




/// Output Half Barrel information
std::ostream &operator << (std::ostream& os, const AlignableMuBarrel& b )
{

  os << "This MuBarrel contains " << b.theDTWheels.size() << " Barrel wheels" << std::endl;
  os << "(phi, r, z) =  (" << b.globalPosition().phi() << "," 
     << b.globalPosition().perp() << "," << b.globalPosition().z();
  os << "),  orientation:" << std::endl<< b.globalRotation() << std::endl;
  return os;

}


/// Recursive printout of whole Half Barrel structure
void AlignableMuBarrel::dump( void )
{

  std::cout << (*this);
  for ( std::vector<AlignableDTWheel*>::iterator iWheel = theDTWheels.begin();
		iWheel != theDTWheels.end(); iWheel++ )
	(*iWheel)->dump();

}
