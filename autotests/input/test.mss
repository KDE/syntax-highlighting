/* kate: hl CartoCSS
   This file contains some content coming from
   https://github.com/gravitystorm/openstreetmap-carto
   with CC0 license. This file is just for testing
   katepart highlighting engine.
   */

#world {
// this syntax
polygon-opacity: 50%;

// is equivalent to
polygon-opacity: 0.5;
}

@admin-boundaries: #ac46ac;

/* For performance reasons, the admin border layers are split into three groups
for low, middle and high zoom levels.
For each zoomlevel, all borders come from a single attachment, to handle
overlapping borders correctly.
*/

#admin-low-zoom[zoom < 11],  // test
#admin-mid-zoom[zoom >= 11][zoom < 13],
#admin-high-zoom[zoom >= 13] {
  [admin_level = '2'],
  [admin_level = '3'] {
    [zoom >= 4] {
      background/line-color: white;
      background/line-width: 0.6;
      line-color: @admin-boundaries;
      line-width: 0.6;
    }
    [zoom >= 7] {
      background/line-width: 2;
      line-width: 2;
    }
    [zoom >= 10] {
      [admin_level = '2'] {
        background/line-width: 6;
        line-width: 6;
      }
      [admin_level = '3'] {
        background/line-width: 5;
        line-width: 5;
        line-dasharray: 4,2;
        line-clip: false;
      }
    }
  }
  [admin_level = '4'] {
    [zoom >= 4] {
      background/line-color: white;
      background/line-width: 0.6;
      line-color: @admin-boundaries;
      line-width: 0.6;
      line-dasharray: 4,3;
      line-clip: false;
    }
    [zoom >= 7] {
      background/line-width: 1;
      line-width: 1;
    }
    [zoom >= 11] {
      background/line-width: 3;
      line-width: 3;
    }
  }
  /*
  The following code prevents admin boundaries from being rendered on top of
  each other. Comp-op works on the entire attachment, not on the individual
  border. Therefore, this code generates an attachment containing a set of
  @admin-boundaries/white dashed lines (of which only the top one is visible),
  and with `comp-op: darken` the white part is ignored, while the
  @admin-boundaries colored part is rendered (as long as the background is not
  darker than @admin-boundaries).
  The SQL has `ORDER BY admin_level`, so the boundary with the lowest
  admin_level is rendered on top, and therefore the only visible boundary.
  */
  opacity: 0.4;
  comp-op: darken;
}

#admin-mid-zoom[zoom >= 11][zoom < 13],
#admin-high-zoom[zoom >= 13] {
  [admin_level = '5'][zoom >= 11] {
    background/line-color: white;
    background/line-width: 2;
    line-color: @admin-boundaries;
    line-width: 2;
    line-dasharray: 6,3,2,3,2,3;
    line-clip: false;
  }
  [admin_level = '6'][zoom >= 11] {
    background/line-color: white;
    background/line-width: 2;
    line-color: @admin-boundaries;
    line-width: 2;
    line-dasharray: 6,3,2,3;
    line-clip: false;
  }
  [admin_level = '7'],
  [admin_level = '8'] {
    [zoom >= 12] {
      background/line-color: white;
      background/line-width: 1.5;
      line-color: @admin-boundaries;
      line-width: 1.5;
      line-dasharray: 5,2;
      line-clip: false;
    }
  }
  opacity: 0.5;
  comp-op: darken;
}

#admin-high-zoom[zoom >= 13] {
  [admin_level = '9'],
  [admin_level = '10'] {
    [zoom >= 13] {
      background/line-color: white;
      background/line-width: 2;
      line-color: @admin-boundaries;
      line-width: 2;
      line-dasharray: 2,3;
      line-clip: false;
    }
  }
  opacity: 0.5;
  comp-op: darken;
}



#nature-reserve-boundaries {
  [way_pixels > 100][zoom >= 7] {
    [zoom < 10] {
      ::fill {
        opacity: 0.05;
        polygon-fill: green;
      }
    }
    a/line-width: 1;
    a/line-offset: -0.5;
    a/line-color: green;
    a/line-opacity: 0.15;
    a/line-join: round;
    a/line-cap: round;
    b/line-width: 2;
    b/line-offset: -1;
    b/line-color: green;
    b/line-opacity: 0.15;
    b/line-join: round;
    b/line-cap: round;
    [zoom >= 10] {
      a/line-width: 2;
      a/line-offset: -1;
      b/line-width: 4;
      b/line-offset: -2;
    }
    [zoom >= 14] {
      b/line-width: 6;
      b/line-offset: -3;
    }
  }
}

#building-text {
[zoom >= 14][way_pixels > 3000],
[zoom >= 17] {
text-name: "[name]";
text-size: 11;
text-fill: #444;
text-face-name: @book-fonts;
text-halo-radius: 1;
text-wrap-width: 20;
text-halo-fill: rgba(255,255,255,0.5);
text-placement: interior;
}
}

@marina-text: #576ddf; // also swimming_pool
@landcover-face-name: @oblique-fonts;
@standard-wrap-width: 30;

.points {
  [feature = 'tourism_alpine_hut'][zoom >= 13] {
    point-file: url('symbols/alpinehut.p.16.png');
    point-placement: interior;
  }
  }

  [feature = 'highway_bus_stop'] {
    [zoom >= 16] {
      marker-file: url('symbols/square.svg');
      marker-fill: @transportation-icon;
      marker-placement: interior;
      marker-width: 6;
    }
    [zoom >= 17] {
      marker-file: url('symbols/bus_stop.p.12.png');
      marker-width: 12;
    }
  }

[feature = 'highway_primary'] {
[zoom >= 7][zoom < 12] {
line-width: 0.5;
line-color: @primary-fill;
[zoom >= 9] { line-width: 1.2; }
[zoom >= 10] { line-width: 2; }
[zoom >= 11] { line-width: .5; }
}
}
