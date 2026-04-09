#!/usr/bin/env python3
"""Panda Papers Gallery — the contact book of roasted souls."""

import os
import sys
import json
import random
import uuid
import html as h
import base64

# ============================================================
# CONFIG
# ============================================================

METHOD = os.environ.get("REQUEST_METHOD", "GET")
CONTENT_LENGTH = int(os.environ.get("CONTENT_LENGTH", 0))

def get_upload_dir():
    d = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "upload", "selfies")
    if not os.path.isdir(d):
        d = "/app/www/upload/selfies"
    return d

# ============================================================
# POST — PIN AN ENTRY
# ============================================================

if METHOD == "POST" and CONTENT_LENGTH > 0:
    fp = sys.stdin.buffer if hasattr(sys.stdin, 'buffer') else sys.stdin
    body = fp.read(CONTENT_LENGTH)
    if isinstance(body, bytes):
        body = body.decode('utf-8', errors='replace')

    from urllib.parse import unquote

    # Parse POST params
    params = {}
    for part in body.split('&'):
        if '=' in part:
            k, v = part.split('=', 1)
            params[k] = unquote(v)

    # DELETE action: delete=<entry_id>
    if 'delete' in params:
        entry_id = params['delete']
        if entry_id and all(c in 'abcdef0123456789-' for c in entry_id):
            upload_dir = get_upload_dir()
            json_path = os.path.join(upload_dir, entry_id + '.json')
            if os.path.isfile(json_path):
                try:
                    with open(json_path) as f:
                        data = json.load(f)
                    photo_url = data.get('photo', '')
                    if photo_url:
                        photo_fname = photo_url.split('/')[-1]
                        photo_path = os.path.join(upload_dir, photo_fname)
                        if os.path.isfile(photo_path):
                            os.remove(photo_path)
                except Exception:
                    pass
                os.remove(json_path)
        print("Content-Type: text/plain")
        print("")
        print("ok")
        sys.exit(0)

    # PIN action: pin=<base64data>
    if 'pin' in params:
        try:
            entry = json.loads(base64.b64decode(params['pin']).decode('utf-8'))
            upload_dir = get_upload_dir()
            if not os.path.isdir(upload_dir):
                os.makedirs(upload_dir, exist_ok=True)
            entry_id = str(uuid.uuid4())[:12]
            with open(os.path.join(upload_dir, entry_id + '.json'), 'w') as f:
                json.dump(entry, f)
            print("Content-Type: text/plain")
            print("")
            print("ok")
        except Exception:
            print("Content-Type: text/plain")
            print("")
            print("error")
        sys.exit(0)

    print("Content-Type: text/plain")
    print("")
    print("no action")
    sys.exit(0)

# ============================================================
# LOAD ENTRIES
# ============================================================

def load_entries():
    upload_dir = get_upload_dir()
    if not os.path.isdir(upload_dir):
        return []
    entries = []
    for fname in sorted(os.listdir(upload_dir), reverse=True):
        if not fname.endswith('.json'):
            continue
        try:
            with open(os.path.join(upload_dir, fname)) as f:
                entry = json.load(f)
                entry['_id'] = fname[:-5]
                entries.append(entry)
        except (json.JSONDecodeError, IOError):
            continue
    return entries

# ============================================================
# RANDOM DECORATIONS
# ============================================================

random.seed(random.randint(0, 9999))

STICKER_EMOJIS = ["&#128060;","&#127843;","&#127881;","&#127829;","&#11088;","&#128165;",
                   "&#127918;","&#128293;","&#127844;","&#128062;","&#9889;","&#127880;"]
SPLATTER_COLORS = ["rgba(255,0,110,0.12)","rgba(131,56,236,0.10)","rgba(58,134,255,0.10)",
                    "rgba(255,230,109,0.12)","rgba(0,245,160,0.10)"]
TAPE_LABELS = ["busted", "cooked", "roasted", "victim", "ouch", "rip", "lol", "oof"]

def gen_stickers_html(n):
    parts = []
    for _ in range(n):
        parts.append('<div class="bg-sticker" style="left:{}%;top:{}%;transform:rotate({}deg) scale({});">{}</div>'.format(
            random.randint(2, 92), random.randint(2, 90), random.randint(-35, 35),
            round(random.uniform(0.8, 2.0), 2), random.choice(STICKER_EMOJIS)))
    return '\n'.join(parts)

def gen_splatters(n):
    css_parts, html_parts = [], []
    for i in range(n):
        x, y = random.randint(0, 90), random.randint(0, 85)
        size = random.randint(80, 300)
        c = random.choice(SPLATTER_COLORS)
        css_parts.append('.splat-{}{{position:fixed;left:{}%;top:{}%;width:{}px;height:{}px;background:{};'
                         'border-radius:50% 40% 60% 35%;filter:blur(40px);pointer-events:none;z-index:0;}}'.format(
                             i, x, y, size, size, c))
        html_parts.append('<div class="splat-{}"></div>'.format(i))
    return '\n'.join(css_parts), '\n'.join(html_parts)

sticker_html = gen_stickers_html(14)
splatter_css, splatter_html = gen_splatters(8)

# ============================================================
# GET — BUILD GALLERY PAGE
# ============================================================

entries = load_entries()

cards_html = ""
if not entries:
    cards_html = '<div class="empty-msg">&#128060; no one has been pinned yet... go roast someone and hit anheften!</div>'
else:
    for entry in entries:
        rot = random.randint(-4, 4)
        tape_r = random.randint(-10, 10)
        name = h.escape(entry.get('name', '???'))
        age = h.escape(str(entry.get('age', '?')))
        occ = h.escape(entry.get('occupation', ''))
        lang = h.escape(entry.get('language', ''))
        roast_text = h.escape(entry.get('roast', ''))
        photo = entry.get('photo')
        label = random.choice(TAPE_LABELS)
        eid = h.escape(entry.get('_id', ''))

        if photo:
            img_html = '<img src="{}" alt="{}">'.format(h.escape(photo), name)
        else:
            img_html = '<div class="no-face">&#128060;</div>'

        cards_html += """
<div class="card" id="card-{eid}" style="transform:rotate({rot}deg);">
  <div class="card-tape" style="transform:rotate({tape_r}deg);">{label}</div>
  <button class="card-x" onclick="deleteEntry('{eid}', this)" title="entfernen">&#10005;</button>
  <div class="card-photo">{img_html}</div>
  <div class="card-name">{name}</div>
  <div class="card-info">{age} yrs &bull; {occ} &bull; {lang}</div>
  <div class="card-roast">&ldquo;{roast}&rdquo;</div>
</div>""".format(rot=rot, tape_r=tape_r, label=label, img_html=img_html,
                 name=name, age=age, occ=occ, lang=lang, roast=roast_text, eid=eid)

print("Content-Type: text/html")
print("")

print("""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>The Panda's Contact Book</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Caveat:wght@400;600;700&family=Permanent+Marker&family=Patrick+Hand&family=Homemade+Apple&display=swap');
  * {{ margin:0; padding:0; box-sizing:border-box; }}
  html,body {{ min-height:100vh; overflow-x:hidden; }}
  body {{
    background: #f5f0e8;
    background-image:
      repeating-linear-gradient(0deg, transparent, transparent 27px, rgba(180,160,140,0.13) 27px, rgba(180,160,140,0.13) 28px),
      repeating-linear-gradient(90deg, transparent, transparent 27px, rgba(180,160,140,0.07) 27px, rgba(180,160,140,0.07) 28px);
    font-family: 'Patrick Hand', cursive;
    padding: 2rem;
  }}
  .bg-sticker {{ position: fixed; font-size: 2.2rem; pointer-events: none; z-index: 0; opacity: 0.35; filter: saturate(1.4); }}
  {splatter_css}

  .page-header {{
    text-align: center; position: relative; z-index: 2; margin-bottom: 2rem;
  }}
  .page-header .panda-mascot {{ font-size: 3.5rem; display: inline-block; animation: pandaWobble 3s ease-in-out infinite; filter: drop-shadow(2px 3px 0 rgba(0,0,0,0.1)); }}
  @keyframes pandaWobble {{ 0%,100% {{ transform: rotate(-8deg) translateY(0); }} 25% {{ transform: rotate(5deg) translateY(-5px); }} 50% {{ transform: rotate(-3deg) translateY(2px); }} 75% {{ transform: rotate(6deg) translateY(-3px); }} }}
  .page-header h1 {{ font-family: 'Permanent Marker', cursive; font-size: 2rem; color: #1a1a2e; text-shadow: 2px 2px 0 rgba(255,0,110,0.12); }}
  .page-header .subtitle {{ font-family: 'Caveat', cursive; font-size: 1.15rem; color: #7a6a5a; margin-top: 0.3rem; }}
  .counter {{ display: inline-block; background: #1a1a2e; color: #ffe66d; font-family: 'Permanent Marker', cursive; font-size: 0.9rem; padding: 0.2rem 0.8rem; border-radius: 20px; margin-top: 0.5rem; }}

  .grid {{
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(240px, 1fr));
    gap: 2rem;
    max-width: 1100px;
    margin: 0 auto;
    position: relative; z-index: 2;
  }}

  .card {{
    background: #fefcf3;
    border-radius: 4px 10px 10px 4px;
    padding: 1.2rem 1rem 1rem 1.5rem;
    box-shadow: 3px 3px 0 rgba(0,0,0,0.06), 0 8px 25px rgba(0,0,0,0.1);
    border-left: 4px solid #e8d5b8;
    position: relative;
    transition: transform 0.3s, box-shadow 0.2s, opacity 0.3s;
  }}
  .card::before {{ content: ''; position: absolute; left: 12px; top: 0; bottom: 0; width: 2px;
    background: repeating-linear-gradient(to bottom, transparent, transparent 22px, #d4c4a8 22px, #d4c4a8 26px, transparent 26px, transparent 36px); }}
  .card:hover {{ transform: rotate(0deg) scale(1.03) !important; box-shadow: 5px 5px 0 rgba(0,0,0,0.1), 0 12px 30px rgba(0,0,0,0.15); z-index: 10; }}

  .card-tape {{ position: absolute; top: -10px; left: 50%; margin-left: -32px; width: 64px; height: 20px;
    background: rgba(255,255,220,0.6); border: 1px solid rgba(200,190,160,0.3); box-shadow: 0 1px 3px rgba(0,0,0,0.06);
    font-family: 'Homemade Apple', cursive; font-size: 0.45rem; color: #8a7a6a; text-align: center; line-height: 20px; z-index: 5; }}

  .card-x {{ position: absolute; top: 6px; right: 6px; width: 22px; height: 22px; border-radius: 50%;
    background: #d94040; color: #fff; border: none; cursor: pointer; font-size: 0.65rem; line-height: 22px;
    text-align: center; padding: 0; z-index: 6; opacity: 0; transition: opacity 0.2s, transform 0.15s;
    box-shadow: 1px 1px 3px rgba(0,0,0,0.2); }}
  .card:hover .card-x {{ opacity: 1; }}
  .card-x:hover {{ transform: scale(1.15); }}

  .card-photo {{ width: 90px; height: 90px; margin: 0 auto 0.6rem; background: #fff; padding: 4px; box-shadow: 2px 2px 0 rgba(0,0,0,0.06); }}
  .card-photo img {{ width: 100%; height: 100%; object-fit: cover; display: block; filter: contrast(1.05) saturate(1.1); }}
  .card-photo .no-face {{ width: 100%; height: 100%; display: flex; align-items: center; justify-content: center; font-size: 2.5rem; background: #f0ebe3; }}

  .card-name {{ font-family: 'Permanent Marker', cursive; font-size: 1.1rem; color: #1a1a2e; text-align: center; margin-bottom: 0.2rem; }}
  .card-info {{ font-family: 'Homemade Apple', cursive; font-size: 0.55rem; color: #8a7a6a; text-align: center; line-height: 1.6; margin-bottom: 0.5rem; }}
  .card-roast {{ font-family: 'Caveat', cursive; font-size: 0.9rem; color: #4a3000; background: #fff9c4; border: 1px dashed #e8a020;
    border-radius: 2px; padding: 0.5rem 0.6rem; line-height: 1.4; }}

  .empty-msg {{ text-align: center; font-family: 'Caveat', cursive; font-size: 1.5rem; color: #7a6a5a; padding: 3rem; grid-column: 1 / -1; }}

  .back-btn {{ display: inline-block; padding: 0.6rem 1.8rem; background: #1a1a2e; color: #ffe66d; font-family: 'Permanent Marker', cursive; font-size: 1rem; text-decoration: none; border-radius: 30px 5px 30px 5px; transform: rotate(-1deg); box-shadow: 3px 3px 0 rgba(0,0,0,0.15); transition: transform 0.2s, box-shadow 0.2s; }}
  .back-btn:hover {{ transform: rotate(1deg) scale(1.06); box-shadow: 5px 5px 0 rgba(0,0,0,0.2); }}
  .nav {{ text-align: center; position: relative; z-index: 2; margin-top: 2rem; }}
</style></head>
<body>
{splatter_html}
{sticker_html}

<div class="page-header">
  <div class="panda-mascot">&#128060;&#128214;</div>
  <h1>THE PANDA'S CONTACT BOOK</h1>
  <div class="subtitle">every soul the panda has roasted, pinned to the wall of shame</div>
  <div class="counter">&#128204; {count} pinned</div>
</div>

<div class="grid" id="gallery">
{cards}
</div>

<div class="nav">
  <a class="back-btn" href="/website_name/CGI/contact.py">&#128062; roast someone</a>
</div>

<script>
function deleteEntry(id, btn) {{
  var card = btn.closest('.card');
  card.style.opacity = '0';
  card.style.transform += ' scale(0.8)';
  setTimeout(function() {{
    card.remove();
    // Update counter
    var remaining = document.querySelectorAll('.card').length;
    var counter = document.querySelector('.counter');
    if (counter) counter.innerHTML = '&#128204; ' + remaining + ' pinned';
    if (remaining === 0) {{
      document.getElementById('gallery').innerHTML = '<div class="empty-msg">&#128060; no one has been pinned yet... go roast someone and hit anheften!</div>';
    }}
  }}, 300);
  // Send delete request via POST
  var xhr = new XMLHttpRequest();
  xhr.open('POST', '/website_name/CGI/gallery.py', true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  xhr.send('delete=' + encodeURIComponent(id));
}}

document.addEventListener('mousemove', function(e) {{
  var sx = (e.clientX / window.innerWidth - 0.5) * 20;
  var sy = (e.clientY / window.innerHeight - 0.5) * 20;
  var stickers = document.querySelectorAll('.bg-sticker');
  for (var i = 0; i < stickers.length; i++) {{
    var depth = (i % 3 + 1) * 0.4;
    stickers[i].style.marginLeft = (sx * depth) + 'px';
    stickers[i].style.marginTop = (sy * depth) + 'px';
  }}
}});
</script>
</body></html>""".format(
    splatter_css=splatter_css, splatter_html=splatter_html,
    sticker_html=sticker_html, count=len(entries), cards=cards_html))
