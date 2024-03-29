//
//  SpriteSheet.h
//  OGLGame
//
//  Created by Michael Daley on 06/03/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Image.h"

// Class which allows a single image to be referenced as smaller images (texture atlas).
// Currently this class only supports a sprite sheet which contains images that are all of
// the same dimensions i.e high and width.  By specifying the height and width of these
// sprites this class is able to provide access to the smaller images by providing a simple
// X and Y coordinate of the image required.
//
@interface SpriteSheet : NSObject {

	// Sprite sheet name
	NSString *spriteSheetName;
	// Image to be used within this sprite sheet
	Image *image;
	// Sprite width
	GLuint spriteWidth;
	// Sprite height
	GLuint spriteHeight;
	// Spacing between each sprite
	GLuint spacing;
	// Scale of spritesheet
	CGSize scale;
	// Horizontal count
	int horizontal;
	// Vertcal count
	int vertical;
	// Texture coordinate array
    Quad2f *cachedTextureCoordinates;
	
}

@property(nonatomic, readonly)NSString *spriteSheetName;
@property(nonatomic, readonly)Image *image;
@property(nonatomic, readonly)GLuint spriteWidth;
@property(nonatomic, readonly)GLuint spriteHeight;
@property(nonatomic, readonly)GLuint spacing;
@property(nonatomic, readonly)CGSize scale;
@property(nonatomic, readonly)int horizontal;
@property(nonatomic, readonly)int vertical;

// Selector that allows a sprite sheet to be created based on an image which has already been defined
- (id)initWithImage:(Image*)aSpriteSheet spriteWidth:(GLuint)aSpriteWidth spriteHeight:(GLuint)aSpriteHeight spacing:(GLuint)aSpacing;

// Selector that allows a sprite sheet to be created based on the name of an image.  The image
// name is used to create the image used by the sprite sheet class as a sprite sheet.
- (id)initWithImageNamed:(NSString*)aImageName spriteWidth:(GLuint)aSpriteWidth spriteHeight:(GLuint)aSpriteHeight spacing:(GLuint)aSpacing imageScale:(CGSize)scale;

// Returns the sprite image located at the x and y coordinates provided
- (Image*)getSpriteAtX:(GLuint)x y:(GLuint)y;

// Renders a sprite at the location provided at the point provided
- (void)renderSpriteAtX:(GLuint)x y:(GLuint)y point:(CGPoint)aPoint centerOfImage:(BOOL)aCenter;

// Gets the offset of a subimage within the sprite sheet based on the location provided
- (CGPoint)getOffsetForSpriteAtX:(int)x y:(int)y;

// Gets the texture coordinates for a sprite at the location provided
- (Quad2f)getTextureCoordsForSpriteAtX:(GLuint)x y:(GLuint)y;

// Gets the vertices for a sprite at the coordinates provided.
- (Quad2f*)getVerticesForSpriteAtX:(GLuint)x y:(GLuint)y point:(CGPoint)aPoint centerOfImage:(BOOL)aCenter;

@end
