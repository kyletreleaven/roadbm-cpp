/*
 *  Copyright (C) 2012 Josh Bialkowski (jbialk@mit.edu)
 *
 *  This file is part of mpblocks.
 *
 *  mpblocks is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mpblocks is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mpblocks.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 *  @file   /home/josh/Codes/cpp/roadbm-cpp/test/linarr.cpp
 *
 *  @date   Sep 16, 2013
 *  @author Josh Bialkowski (jbialk@mit.edu)
 *  @brief  
 */

#include "linarr.hpp"


int main( int argc, char** argv )
{
    LinearArrangement<> arr;
    for(int i=0; i < 10; i++)
        arr.vertex( rand() % 100 );
    return 0;
}
