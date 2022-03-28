#ifndef _COLORKEY_H
#define _COLORKEY_H

/**
* This function will colorkey an SDL surface and check its success
* @param {bool} enable - Whether or not we're colorkeying this surface
* @param {SDL_Surface* } - surface we're colorkeying
* @return {bool} - whether or not we were successful in colorkeying (If enabled), will return true by default if disabled
*/
bool color_key_surface ( bool enable, SDL_Surface* surface){

  // Check if we're even colorkeying anything
  if (enable){

    // Colorkey and return result
    int result = SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));

    // Checking result...
    if ( result != 0 ){
      // If we don't get a zero (Success) then...
      printf("Unable to colorkey surface!");
      return false;
    }
  }
  // If all else goes well, return true
  return true;
}

#endif
