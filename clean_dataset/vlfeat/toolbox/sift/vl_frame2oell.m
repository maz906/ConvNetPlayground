function eframes = vl_frame2oell(frames)
% FRAMES2OELL   Convert generic feature frames to oriented ellipses
%   EFRAMES = VL_FRAME2OELL(FRAMES) converts the specified FRAMES to
%   the oriented ellipses EFRAMES.
%
%   A frame is either a point, disc, oriented disc, ellipse, or
%   oriented ellipse. These are represened respecively by
%   2, 3, 4, 5 and 6 parameters each, as described in VL_PLOTFRAME().
%
%   An oriented ellipse is the most general frame. When an unoriented
%   frame is converted to an oriented ellipse, the rotation is selected
%   so that the positive Y direction is unchanged.
%
%   See: VL_PLOTFRAME(), VL_HELP().

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

[D,K] = size(frames) ;
eframes = zeros(6,K) ;

switch D
  case 2
    eframes(1:2,:) = frames(1:2,:) ;

  case 3
    eframes(1:2,:) = frames(1:2,:) ;
    eframes(3,:)   = frames(3,:) ;
    eframes(6,:)   = frames(3,:) ;

  case 4
    r = frames(3,:) ;
    c = r.*cos(frames(4,:)) ;
    s = r.*sin(frames(4,:)) ;

    eframes(1:2,:) = frames(1:2,:) ;
    eframes(3:6,:) = [c ; s ; -s ; c] ;

  case 5
    eframes(1:2,:) = frames(1:2,:) ;
    eframes(3:6,:) = mapFromS(frames(3:5,:)) ;

  case 6
    eframes = frames ;
  
  otherwise
     error('FRAMES format is unknown.') ;
end

% --------------------------------------------------------------------
function A = mapFromS(S)
% --------------------------------------------------------------------
% Returns the (stacking of the) 2x2 matrix A that maps the unit circle
% into the ellipses satisfying the equation x' inv(S) x = 1. Here S
% is a stacked covariance matrix, with elements S11, S12 and S22.
%
% The goal is to find A such that AA' = S. In order to let the Y
% direction unaffected (upright feature), the assumption is taht
% A = [a b ; 0 c]. Hence
%
%  AA' = [a^2, ab ; ab, b^2+c^2] = S.

A = zeros(4,size(S,2)) ;
a = sqrt(S(1,:));
b = S(2,:) ./ max(a, 1e-18) ;

A(1,:) = a ; 
A(2,:) = b ;
A(4,:) = sqrt(max(S(3,:) - b.*b, 0)) ;
