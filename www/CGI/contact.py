#!/usr/bin/env python3
"""Panda Papers — a CGI contact form that secretly roasts you."""

import os
import sys
import json
import random
import uuid
import cgi

try:
    from urllib.request import Request, urlopen
except ImportError:
    pass

# ============================================================
# CGI INPUT PARSING
# ============================================================

METHOD = os.environ.get("REQUEST_METHOD", "GET")
CONTENT_TYPE = os.environ.get("CONTENT_TYPE", "")
CONTENT_LENGTH = int(os.environ.get("CONTENT_LENGTH", 0))

form = None
if METHOD == "POST" and CONTENT_LENGTH > 0:
    fp = sys.stdin.buffer if hasattr(sys.stdin, 'buffer') else sys.stdin
    form = cgi.FieldStorage(
        fp=fp,
        environ={
            'REQUEST_METHOD': 'POST',
            'CONTENT_TYPE': CONTENT_TYPE,
            'CONTENT_LENGTH': str(CONTENT_LENGTH),
        },
        keep_blank_values=True
    )

def get_param(name):
    if form is None:
        return ""
    val = form.getfirst(name, "")
    if isinstance(val, bytes):
        val = val.decode("utf-8", errors="replace")
    return val

# Photo stream can only be read once — cache the result
_photo_cache = None

def get_upload():
    """Returns (file_bytes, filename) or (None, None). Caches result."""
    global _photo_cache
    if _photo_cache is not None:
        return _photo_cache
    _photo_cache = (None, None)
    if form is None or "photo" not in form:
        return _photo_cache
    item = form["photo"]
    if isinstance(item, list):
        item = item[0]
    if not getattr(item, 'file', None) or not item.filename:
        return _photo_cache
    data = item.file.read()
    if len(data) >= 100:
        _photo_cache = (data, item.filename)
    return _photo_cache

# ============================================================
# VALIDATION & LIMITS
# ============================================================

MAX_NAME = 50
MAX_OCCUPATION = 80
MAX_HOBBY = 80
MAX_LANG = 30
MAX_PHOTO = 1500000
MIN_AGE, MAX_AGE = 1, 120
ALLOWED_IMG_SIGS = [b'\xff\xd8\xff', b'\x89PNG', b'GIF8']

def resolve_language():
    """Resolve language from form, handling 'Other' + custom input."""
    lang = get_param("language").strip()
    if lang == "Other":
        lang = get_param("custom_language").strip()
    return lang[:MAX_LANG] if lang else "Python"

def get_form_data():
    """Extract and sanitize all form fields. Returns dict."""
    return {
        'name': get_param("name").strip()[:MAX_NAME],
        'age': get_param("age").strip(),
        'occupation': get_param("occupation").strip()[:MAX_OCCUPATION],
        'hobby': get_param("hobby").strip()[:MAX_HOBBY],
        'language': resolve_language(),
    }

def validate(data):
    """Returns (ok, error_message)."""
    if not data['name']:
        return (False, "You forgot your name! The panda needs to know who you are.")
    if len(data['name']) > MAX_NAME:
        return (False, "Your name is {} chars. Max is {}. Even pandas have limits.".format(len(data['name']), MAX_NAME))
    if not data['age']:
        return (False, "Age is required. The panda needs to know how long you've been making bad decisions.")
    try:
        age = int(data['age'])
    except ValueError:
        return (False, "'{}' is not a number. Nice try.".format(data['age'][:20]))
    if age < MIN_AGE or age > MAX_AGE:
        return (False, "Age must be between {} and {}. No time travelers or immortals allowed.".format(MIN_AGE, MAX_AGE))
    if not data['occupation']:
        return (False, "What do you do? 'Nothing' is also an answer, but you gotta type it.")
    if len(data['occupation']) > MAX_OCCUPATION:
        return (False, "Your job title is {} chars. Keep it under {}. This isn't LinkedIn.".format(len(data['occupation']), MAX_OCCUPATION))
    if not data['hobby']:
        return (False, "No hobby? That's sad. But also you need to fill this in.")
    if len(data['hobby']) > MAX_HOBBY:
        return (False, "Your hobby description is {} chars. Max {}. Keep it short, bamboo brain.".format(len(data['hobby']), MAX_HOBBY))
    if len(data['language']) > MAX_LANG:
        return (False, "Language name too long. Keep it under {} characters.".format(MAX_LANG))

    photo_data, _ = get_upload()
    if photo_data is not None:
        if len(photo_data) > MAX_PHOTO:
            return (False, "Photo is too large ({:.1f}MB). Max is {:.1f}MB. Compress it or take a smaller selfie.".format(
                len(photo_data) / 1e6, MAX_PHOTO / 1e6))
        if not any(photo_data[:len(sig)] == sig for sig in ALLOWED_IMG_SIGS):
            return (False, "That doesn't look like an image. Only JPEG, PNG, and GIF are allowed.")
    return (True, "")

# ============================================================
# PHOTO UPLOAD
# ============================================================

def get_upload_dir():
    d = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "upload", "selfies")
    if not os.path.isdir(d):
        d = "/app/www/upload/selfies"
    if not os.path.isdir(d):
        os.makedirs(d, exist_ok=True)
    return d

def save_photo():
    """Save uploaded photo, return URL path or None."""
    photo_data, _ = get_upload()
    if photo_data is None:
        return None
    ext = ".png" if photo_data[:4] == b'\x89PNG' else ".gif" if photo_data[:4] == b'GIF8' else ".jpg"
    filename = str(uuid.uuid4())[:12] + ext
    upload_dir = get_upload_dir()
    with open(os.path.join(upload_dir, filename), 'wb') as f:
        f.write(photo_data)
    return "/website_name/upload/selfies/" + filename

def save_entry(data, photo_url, roast_text):
    """Save contact book entry as JSON alongside photos."""
    upload_dir = get_upload_dir()
    entry_id = str(uuid.uuid4())[:12]
    entry = {
        'name': data['name'],
        'age': data['age'],
        'occupation': data['occupation'],
        'hobby': data['hobby'],
        'language': data['language'],
        'photo': photo_url,
        'roast': roast_text,
    }
    with open(os.path.join(upload_dir, entry_id + '.json'), 'w') as f:
        json.dump(entry, f)

# ============================================================
# ROAST ENGINE
# ============================================================

FALLBACK_TEMPLATES = [
    "So {name}, you're {age} years old and your biggest achievement is listing '{hobby}' as a hobby? Bold move putting that on a form. Most people at least pretend to have their life together before submitting personal data to a panda.",
    "{name} is a {occupation} who codes in {language}. That's not a career, that's a cry for help. The panda has seen some things, but a {age}-year-old whose hobby is '{hobby}'? That's a new low.",
    "A {age}-year-old {occupation} whose hobby is '{hobby}'? {name}, even the panda is embarrassed for you. You typed that with your whole chest and hit submit like it was something to be proud of.",
    "{name} chose {language} as their favorite language. At {age}, you'd think they'd have learned better by now. But no, here you are, a {occupation} who does '{hobby}' for fun. The panda is genuinely concerned.",
    "Let me get this straight, {name}: you're {age}, you work as a {occupation}, and in your free time you do '{hobby}'? The bar was on the floor and you brought a shovel. Even {language} developers are looking down on you right now.",
    "Ah yes, {name} the {occupation}. {age} years on this planet and '{hobby}' is the best you came up with. {language} fans really are a different breed — and not in a good way. The panda would roast you harder but honestly feels bad.",
    "{name}, a {age}-year-old {occupation} who enjoys '{hobby}' and codes in {language}. If mediocrity had a LinkedIn, this would be the featured post. The panda has roasted thousands and you still managed to stand out — just not in the way you'd hope.",
    "Dear {name}, at {age} most people have figured life out. You chose {language} and '{hobby}'. The panda weeps. Being a {occupation} is fine, but combining it with everything else on this form? That's a lifestyle choice the panda cannot endorse.",
]

def roast(data):
    """Get a roast — from API if available, otherwise from templates."""
    api_key = os.environ.get("OPENAI_API_KEY", "")
    if not api_key:
        return random.choice(FALLBACK_TEMPLATES).format(**data)

    prompt = (
        "You are a savage but funny roast comedian. "
        "Someone just filled out a 'contact form' thinking it was serious. "
        "Roast them in 2-3 sentences based on this info. Be creative and brutal but keep it lighthearted. "
        "Do NOT be mean about things people can't change. Focus on their choices.\n\n"
        "Name: {name}\nAge: {age}\nOccupation: {occupation}\n"
        "Favorite Hobby: {hobby}\nFavorite Programming Language: {language}"
    ).format(**data)

    payload = json.dumps({
        "model": "gpt-4o-mini",
        "messages": [{"role": "user", "content": prompt}],
        "max_tokens": 200,
        "temperature": 1.0
    }).encode("utf-8")

    req = Request("https://api.openai.com/v1/chat/completions", data=payload, method="POST")
    req.add_header("Content-Type", "application/json")
    req.add_header("Authorization", "Bearer " + api_key)

    try:
        resp = urlopen(req, timeout=10)
        result = json.loads(resp.read().decode("utf-8"))
        return result["choices"][0]["message"]["content"]
    except Exception:
        return random.choice(FALLBACK_TEMPLATES).format(**data)

# ============================================================
# RANDOMIZED UI ELEMENTS
# ============================================================

random.seed(random.randint(0, 9999))

MICROCOPY = {
    'headlines': [
        "JOIN THE PANDA PAPERS!",
        "SLAP A STICKER, JOIN THE GANG",
        "SIGN HERE, BRING BAMBOO",
        "WELCOME TO PANDA HQ",
        "THE PANDA WANTS YOUR SECRETS",
    ],
    'name_prompts': [
        "What do your friends call you?",
        "Your name (or alias, we don't judge)",
        "Who even are you?",
        "Name, please. The panda is waiting.",
    ],
    'age_prompts': [
        "Bamboo years (approx.)",
        "How old? Be honest.",
        "Age in human years",
        "How many trips around the sun?",
    ],
    'job_prompts': [
        "Secret job when no one's watching",
        "What do you do when not eating bamboo?",
        "Your so-called 'occupation'",
        "Day job (or night job, no judgment)",
    ],
    'hobby_prompts': [
        "Guilty pleasure hobby?",
        "What do you do for fun?",
        "Secret hobby no one knows about",
        "Hobby that keeps you sane (or insane)",
    ],
    'submit_texts': [
        "SLAP IT", "JOIN THE GANG", "SEND BAMBOO", "PUSH THE PAW", "DO IT. NOW.",
    ],
    'roast_headlines': [
        "THE PANDA HAS SPOKEN", "ABSOLUTELY COOKED", "ROASTED BY A BEAR",
        "PANDA SAYS: OUCH", "YOU ASKED FOR THIS",
    ],
    'error_titles': [
        "OOPS. THE PANDA IS CONFUSED.", "NOPE. TRY AGAIN.",
        "THE PANDA REJECTS THIS.", "HOLD UP, BAMBOO BRAIN.", "SOMETHING WENT WRONG.",
    ],
    'victim_captions': [
        "the victim", "exhibit A", "this guy", "roast target", "face of defeat", "caught in 4k",
    ],
}

def pick(key):
    return random.choice(MICROCOPY[key])

# --- Generate random decorations ---

STICKER_EMOJIS = ["&#128060;","&#127843;","&#127881;","&#127829;","&#11088;","&#128165;",
                   "&#127918;","&#128293;","&#127844;","&#128062;","&#9889;","&#127880;"]
SPLATTER_COLORS = ["rgba(255,0,110,0.12)","rgba(131,56,236,0.10)","rgba(58,134,255,0.10)",
                    "rgba(255,230,109,0.12)","rgba(0,245,160,0.10)"]

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
tape_rot1, tape_rot2 = random.randint(-8, 8), random.randint(-8, 8)
nb_rot = round(random.uniform(-4, -1.5), 1)
nb_shift_x, nb_shift_y = random.randint(-30, 10), random.randint(-20, 10)

# ============================================================
# SHARED HTML BUILDING BLOCKS
# ============================================================

NOTEBOOK_BASE_CSS = """
  @import url('https://fonts.googleapis.com/css2?family=Caveat:wght@400;600;700&family=Permanent+Marker&family=Patrick+Hand&family=Homemade+Apple&display=swap');
  * {{ margin:0; padding:0; box-sizing:border-box; }}
  html,body {{ min-height:100vh; overflow-x:hidden; }}
  body {{
    background: #f5f0e8;
    background-image:
      repeating-linear-gradient(0deg, transparent, transparent 27px, rgba(180,160,140,0.13) 27px, rgba(180,160,140,0.13) 28px),
      repeating-linear-gradient(90deg, transparent, transparent 27px, rgba(180,160,140,0.07) 27px, rgba(180,160,140,0.07) 28px);
    font-family: 'Patrick Hand', cursive;
    display: flex; justify-content: center; align-items: center;
    padding: 2rem; min-height: 100vh;
  }}
  .bg-sticker {{ position: fixed; font-size: 2.2rem; pointer-events: none; z-index: 0; opacity: 0.35; filter: saturate(1.4); transition: margin 0.4s ease-out; }}
  .tape {{ position: absolute; width: 90px; height: 28px; background: rgba(255,255,220,0.55); border: 1px solid rgba(200,190,160,0.3); box-shadow: 0 1px 4px rgba(0,0,0,0.08); z-index: 10; }}
  .notebook {{
    position: relative; z-index: 2; background: #fefcf3; border-radius: 4px 12px 12px 4px;
    padding: 2.5rem 2.2rem 2rem 3.2rem; max-width: 520px; width: 95%;
    transform: rotate({nb_rot}deg) translate({nb_shift_x}px, {nb_shift_y}px);
    box-shadow: 4px 4px 0 rgba(0,0,0,0.06), 8px 8px 0 rgba(0,0,0,0.03), 0 15px 40px rgba(0,0,0,0.12);
    border-left: 4px solid #e8d5b8;
  }}
  .notebook::before {{ content: ''; position: absolute; left: 14px; top: 0; bottom: 0; width: 2px;
    background: repeating-linear-gradient(to bottom, transparent, transparent 22px, #d4c4a8 22px, #d4c4a8 26px, transparent 26px, transparent 36px); }}
  .notebook::after {{ content: ''; position: absolute; top: -6px; left: 20px; right: 10px; height: 12px;
    background: linear-gradient(135deg, #fefcf3 33%, transparent 33%) -7px 0, linear-gradient(225deg, #fefcf3 33%, transparent 33%) -7px 0; background-size: 14px 12px; }}
  .panda-mascot {{ font-size: 3.5rem; display: inline-block; animation: pandaWobble 3s ease-in-out infinite; filter: drop-shadow(2px 3px 0 rgba(0,0,0,0.1)); }}
  @keyframes pandaWobble {{ 0%,100% {{ transform: rotate(-8deg) translateY(0); }} 25% {{ transform: rotate(5deg) translateY(-5px); }} 50% {{ transform: rotate(-3deg) translateY(2px); }} 75% {{ transform: rotate(6deg) translateY(-3px); }} }}
  h1 {{ font-family: 'Permanent Marker', cursive; font-size: 1.7rem; color: #1a1a2e; transform: rotate(-1.5deg); margin: 0.2rem 0 0.3rem; text-shadow: 2px 2px 0 rgba(255,0,110,0.12); line-height: 1.1; }}
  .ink-smudge {{ position: absolute; width: 60px; height: 40px; background: radial-gradient(ellipse, rgba(30,20,60,0.06) 0%, transparent 70%); pointer-events: none; border-radius: 50%; }}
  .back-btn {{ display: inline-block; margin-top: 1.5rem; padding: 0.6rem 1.8rem; background: #1a1a2e; color: #ffe66d; font-family: 'Permanent Marker', cursive; font-size: 1rem; text-decoration: none; border-radius: 30px 5px 30px 5px; transform: rotate(-1deg); box-shadow: 3px 3px 0 rgba(0,0,0,0.15); transition: transform 0.2s, box-shadow 0.2s; }}
  .back-btn:hover {{ transform: rotate(1deg) scale(1.06); box-shadow: 5px 5px 0 rgba(0,0,0,0.2); }}
"""

PARALLAX_JS = """
<script>
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
"""

def base_css():
    return NOTEBOOK_BASE_CSS.format(nb_rot=nb_rot, nb_shift_x=nb_shift_x, nb_shift_y=nb_shift_y)

def page_shell():
    """Common template vars for all pages."""
    return dict(
        base_css=base_css(), splatter_css=splatter_css, splatter_html=splatter_html,
        sticker_html=sticker_html, tape_rot1=tape_rot1, tape_rot2=tape_rot2,
        parallax_js=PARALLAX_JS,
    )

# ============================================================
# OUTPUT
# ============================================================

print("Content-Type: text/html")
print("")

# --- ERROR PAGE ---
def show_error(msg):
    import html as h
    ctx = page_shell()
    ctx.update(
        et=pick('error_titles'),
        pr=random.choice(["&#128060; &#10060;", "&#128060; &#128172;", "&#128060; &#129300;"]),
        safe_msg=h.escape(msg),
    )
    print("""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>Error - Panda Papers</title>
<style>
  {base_css}
  {splatter_css}
  .error-card {{ position: relative; background: #ffd6d6; border: 3px dashed #d94040; border-radius: 2px; padding: 1.3rem 1.2rem; margin: 1rem -0.5rem; transform: rotate(0.8deg); box-shadow: 3px 3px 0 rgba(0,0,0,0.05); font-family: 'Caveat', cursive; font-size: 1.25rem; line-height: 1.6; color: #6a1a1a; }}
  .error-card::before {{ content: '\\26A0'; position: absolute; top: -16px; left: 15px; font-size: 1.5rem; }}
  .panda-speech {{ position: relative; background: #fff9c4; border-radius: 8px; padding: 0.8rem 1rem; margin-top: 1rem; font-family: 'Patrick Hand', cursive; font-size: 0.95rem; color: #5a4a3a; box-shadow: 2px 2px 0 rgba(0,0,0,0.05); transform: rotate(-0.5deg); }}
  .panda-speech::after {{ content: ''; position: absolute; top: -10px; left: 30px; border-width: 0 8px 10px 8px; border-style: solid; border-color: transparent transparent #fff9c4 transparent; }}
</style></head>
<body>
{splatter_html}
{sticker_html}
<div class="notebook">
  <div class="tape" style="top:-14px;left:25%;transform:rotate({tape_rot1}deg);"></div>
  <div class="tape" style="top:-14px;right:10%;transform:rotate({tape_rot2}deg);"></div>
  <div class="ink-smudge" style="bottom:30px;right:15px;"></div>
  <div style="text-align:center;">
    <div class="panda-mascot" style="animation-duration:1.5s;">{pr}</div>
    <h1>{et}</h1>
  </div>
  <div class="error-card">{safe_msg}</div>
  <div class="panda-speech">&#128172; Don't worry, just go back and fix it. The panda believes in you. Probably.</div>
  <div style="text-align:center;"><a class="back-btn" href="/website_name/CGI/contact.py">&#128062; try again</a></div>
</div>
{parallax_js}
</body></html>""".format(**ctx))


# --- CONFETTI JS (used on roast page) ---
CONFETTI_JS = """
<script>
(function() {{
  var c = document.getElementById('confetti'), ctx = c.getContext('2d');
  c.width = window.innerWidth; c.height = window.innerHeight;
  var shapes = [], colors = ['#ff006e','#8338ec','#3a86ff','#ffe66d','#00f5a0','#ff8c42','#f72585'];
  for (var i = 0; i < 120; i++) {{
    shapes.push({{ x: c.width*0.5+(Math.random()-0.5)*200, y: c.height*0.4, vx: (Math.random()-0.5)*18,
      vy: Math.random()*-16-4, rot: Math.random()*360, vr: (Math.random()-0.5)*12,
      size: Math.random()*10+5, color: colors[Math.floor(Math.random()*colors.length)],
      type: Math.floor(Math.random()*3), life: 1 }});
  }}
  function draw() {{
    ctx.clearRect(0,0,c.width,c.height); var alive = false;
    for (var i=0;i<shapes.length;i++) {{
      var s=shapes[i]; if(s.life<=0) continue; alive=true;
      s.x+=s.vx; s.y+=s.vy; s.vy+=0.35; s.rot+=s.vr; s.life-=0.008; s.vx*=0.99;
      ctx.save(); ctx.translate(s.x,s.y); ctx.rotate(s.rot*Math.PI/180);
      ctx.globalAlpha=Math.max(0,s.life); ctx.fillStyle=s.color;
      if(s.type===0) ctx.fillRect(-s.size/2,-s.size/2,s.size,s.size*0.5);
      else if(s.type===1) {{ ctx.beginPath();ctx.arc(0,0,s.size/2.5,0,Math.PI*2);ctx.fill(); }}
      else {{ ctx.beginPath();ctx.moveTo(0,-s.size/2);ctx.lineTo(s.size/3,s.size/3);ctx.lineTo(-s.size/3,s.size/3);ctx.fill(); }}
      ctx.restore();
    }}
    if(alive) requestAnimationFrame(draw);
  }}
  draw();
  window.addEventListener('resize', function(){{ c.width=window.innerWidth; c.height=window.innerHeight; }});
}})();
</script>
"""

# ============================================================
# POST — ROAST RESULT PAGE
# ============================================================
if METHOD == "POST":
    import html as h

    data = get_form_data()
    ok, err = validate(data)
    if not ok:
        show_error(err)
        sys.exit(0)

    photo_url = save_photo()
    raw_roast = roast(data)
    roast_text = h.escape(raw_roast)
    safe = {k: h.escape(v) for k, v in data.items()}

    # Build JSON blob for the pin button (anheften)
    import base64
    pin_data = base64.b64encode(json.dumps({
        'name': data['name'], 'age': data['age'], 'occupation': data['occupation'],
        'hobby': data['hobby'], 'language': data['language'],
        'photo': photo_url, 'roast': raw_roast,
    }).encode()).decode()

    # Polaroid photo
    photo_rot = random.randint(-8, 8)
    photo_tape_rot = random.randint(-12, 12)
    if photo_url:
        photo_html = """
  <div class="polaroid" style="transform:rotate({rot}deg);" id="polaroid">
    <div class="polaroid-tape" style="transform:rotate({trot}deg);"></div>
    <div class="photo-actions">
      <a class="photo-act dl" href="{url}" download title="download">&#8681;</a>
      <button class="photo-act del" onclick="deletePhoto()" title="delete">&#10005;</button>
    </div>
    <img src="{url}" alt="victim photo" id="photoImg">
    <div class="polaroid-caption">{cap}</div>
  </div>""".format(rot=photo_rot, trot=photo_tape_rot, url=h.escape(photo_url), cap=pick('victim_captions'))
    else:
        photo_html = """
  <div class="polaroid no-photo" style="transform:rotate({rot}deg);">
    <div class="polaroid-tape" style="transform:rotate({trot}deg);"></div>
    <div class="no-photo-face">&#128060;</div>
    <div class="polaroid-caption">no photo on file</div>
  </div>""".format(rot=photo_rot, trot=photo_tape_rot)

    ctx = page_shell()
    ctx.update(rh=pick('roast_headlines'), roast=roast_text, photo_html=photo_html,
               confetti_js=CONFETTI_JS, pin_data=pin_data, **safe)

    print("""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>ROASTED</title>
<style>
  {base_css}
  {splatter_css}
  .notebook {{ max-width: 560px; padding-top: 3rem; }}
  .polaroid {{ position: absolute; top: -18px; right: -12px; background: #fff; padding: 6px 6px 28px 6px; box-shadow: 3px 4px 0 rgba(0,0,0,0.08), 0 8px 20px rgba(0,0,0,0.12); z-index: 15; transition: transform 0.3s; }}
  .polaroid:hover {{ z-index: 20; transform: rotate(0deg) scale(1.15) !important; }}
  .photo-actions {{ position: absolute; top: 4px; right: 4px; display: flex; gap: 3px; z-index: 17; opacity: 0; transition: opacity 0.2s; }}
  .polaroid:hover .photo-actions {{ opacity: 1; }}
  .photo-act {{ width: 24px; height: 24px; border-radius: 50%; border: none; cursor: pointer; font-size: 0.7rem; line-height: 24px; text-align: center; padding: 0; box-shadow: 1px 1px 3px rgba(0,0,0,0.2); transition: transform 0.15s; }}
  .photo-act:hover {{ transform: scale(1.15); }}
  .photo-act.del {{ background: #d94040; color: #fff; }}
  .photo-act.dl {{ background: #3a86ff; color: #fff; text-decoration: none; display: inline-block; }}
  .polaroid-tape {{ position: absolute; top: -8px; left: 50%; margin-left: -35px; width: 70px; height: 22px; background: rgba(255,255,220,0.6); border: 1px solid rgba(200,190,160,0.3); box-shadow: 0 1px 3px rgba(0,0,0,0.06); z-index: 16; }}
  .polaroid img {{ width: 120px; height: 120px; object-fit: cover; display: block; filter: contrast(1.05) saturate(1.1); }}
  .polaroid-caption {{ font-family: 'Homemade Apple', cursive; font-size: 0.55rem; color: #8a7a6a; text-align: center; margin-top: 4px; }}
  .no-photo .no-photo-face {{ width: 120px; height: 120px; display: flex; align-items: center; justify-content: center; font-size: 3rem; background: #f0ebe3; }}
  .header-area {{ text-align: left; padding-right: 140px; margin-bottom: 0.8rem; }}
  .header-area .panda-mascot {{ font-size: 2.8rem; }}
  .header-area h1 {{ font-size: 1.8rem; margin: 0.2rem 0; }}
  .roast-card {{ position: relative; background: #fff3cd; border: 2px dashed #e8a020; border-radius: 2px; padding: 1.3rem 1.2rem; margin: 0.8rem -0.5rem; transform: rotate(0.8deg); box-shadow: 3px 3px 0 rgba(0,0,0,0.05); font-family: 'Caveat', cursive; font-size: 1.3rem; line-height: 1.6; color: #4a3000; }}
  .roast-card::before {{ content: '\\2702'; position: absolute; top: -14px; right: 15px; font-size: 1.3rem; transform: rotate(20deg); }}
  .victim-info {{ font-family: 'Homemade Apple', cursive; font-size: 0.7rem; color: #8a7a6a; margin-top: 0.8rem; transform: rotate(-0.5deg); line-height: 1.8; border-top: 1px dashed rgba(0,0,0,0.1); padding-top: 0.6rem; }}
  #confetti {{ position: fixed; top: 0; left: 0; width: 100%; height: 100%; pointer-events: none; z-index: 100; }}
</style></head>
<body>
{splatter_html}
{sticker_html}
<canvas id="confetti"></canvas>
<div class="notebook">
  <div class="tape" style="top:-14px;left:20%;transform:rotate({tape_rot1}deg);"></div>
  <div class="ink-smudge" style="bottom:20px;right:10px;"></div>
  <div class="ink-smudge" style="top:60px;left:40px;width:40px;height:25px;"></div>
  {photo_html}
  <div class="header-area">
    <div class="panda-mascot">&#128060;</div>
    <h1>{rh} &#128293;</h1>
  </div>
  <div class="roast-card">{roast}</div>
  <div class="victim-info">&#9998; {name}, {age} yrs &bull; {occupation} &bull; {hobby} &bull; {language}</div>
  <div style="text-align:center;display:flex;gap:0.6rem;justify-content:center;flex-wrap:wrap;">
    <a class="back-btn" href="/website_name/CGI/contact.py">&#128062; roast another one</a>
    <button class="back-btn" id="pinBtn" onclick="pinEntry()" style="border:none;cursor:pointer;">&#128204; pin</button>
    <a class="back-btn" href="/website_name/CGI/gallery.py">&#128214; wall of shame</a>
  </div>
  <input type="hidden" id="pinData" value="{pin_data}">
</div>
{confetti_js}
<script>
function deletePhoto() {{
  var img = document.getElementById('photoImg');
  if (!img) return;
  var url = img.getAttribute('src');
  var xhr = new XMLHttpRequest();
  xhr.open('DELETE', url, true);
  xhr.onload = function() {{
    var p = document.getElementById('polaroid');
    if (p) {{ p.style.transition = 'opacity 0.3s, transform 0.3s'; p.style.opacity = '0'; p.style.transform += ' scale(0.8)'; setTimeout(function(){{ p.remove(); }}, 300); }}
  }};
  xhr.onerror = function() {{
    var p = document.getElementById('polaroid');
    if (p) {{ p.style.transition = 'opacity 0.3s, transform 0.3s'; p.style.opacity = '0'; p.style.transform += ' scale(0.8)'; setTimeout(function(){{ p.remove(); }}, 300); }}
  }};
  xhr.send();
}}
function pinEntry() {{
  var btn = document.getElementById('pinBtn');
  var data = document.getElementById('pinData').value;
  var xhr = new XMLHttpRequest();
  xhr.open('POST', '/website_name/CGI/gallery.py', true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  xhr.onload = function() {{
    btn.textContent = '\\uD83D\\uDCCC pinned!';
    btn.style.opacity = '0.6';
    btn.disabled = true;
    btn.onclick = null;
  }};
  xhr.onerror = function() {{
    btn.textContent = '\\u274C fehler';
    btn.style.opacity = '0.6';
  }};
  xhr.send('pin=' + encodeURIComponent(data));
}}
</script>
{parallax_js}
</body></html>""".format(**ctx))

# ============================================================
# GET — FORM PAGE
# ============================================================
else:
    LANG_BADGES = [
        ("Python", "&#128013;", -3), ("JavaScript", "&#9889;", 2), ("C", "&#9881;", -1),
        ("C++", "&#128296;", 3), ("Java", "&#9749;", -2), ("Rust", "&#129408;", 1),
        ("Go", "&#128049;", -4), ("PHP", "&#128584;", 2), ("I dont code", "&#128557;", -1),
        ("Other", "&#9999;", 0),
    ]

    badge_parts = []
    for (lang, emoji, rot) in LANG_BADGES:
        r = rot + random.randint(-2, 2)
        if lang == "Other":
            badge_parts.append('<label class="lang-badge" style="transform:rotate({}deg);" tabindex="0">'
                '<input type="radio" name="language" value="Other" onchange="showCustomLang(true)">'
                '<span class="badge-face">{} {}</span></label>'.format(r, emoji, lang))
        else:
            checked = ' checked' if lang == "Python" else ''
            badge_parts.append('<label class="lang-badge" style="transform:rotate({}deg);" tabindex="0">'
                '<input type="radio" name="language" value="{}"{} onchange="showCustomLang(false)">'
                '<span class="badge-face">{} {}</span></label>'.format(r, lang, checked, emoji, lang))
    badge_html = '\n'.join(badge_parts)

    sn = [round(random.uniform(-4, 4), 1) for _ in range(5)]

    ctx = page_shell()
    ctx.update(
        hl=pick('headlines'), name_prompt=pick('name_prompts'), age_prompt=pick('age_prompts'),
        job_prompt=pick('job_prompts'), hobby_prompt=pick('hobby_prompts'), submit_text=pick('submit_texts'),
        badge_html=badge_html, sn0=sn[0], sn1=sn[1], sn2=sn[2], sn3=sn[3], sn4=sn[4],
    )

    print("""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>Panda Papers</title>
<style>
  {base_css}
  {splatter_css}
  .subtitle {{ font-family: 'Caveat', cursive; font-size: 1.15rem; color: #7a6a5a; transform: rotate(0.5deg); margin-bottom: 0.8rem; }}
  .sticky {{ position: relative; background: #fff9c4; padding: 0.7rem 0.9rem 0.6rem; margin: 0.7rem -0.3rem; box-shadow: 2px 3px 0 rgba(0,0,0,0.06), 0 6px 15px rgba(0,0,0,0.06); border-radius: 1px; }}
  .sticky.blue {{ background: #c8e6ff; }}
  .sticky.pink {{ background: #ffd6e0; }}
  .sticky.green {{ background: #d4f5c4; }}
  .sticky.purple {{ background: #e8d5f5; }}
  .field-label {{ font-family: 'Caveat', cursive; font-size: 1.05rem; color: #5a4a3a; margin-bottom: 0.2rem; display: block; }}
  .char-hint {{ font-family: 'Patrick Hand', cursive; font-size: 0.7rem; color: rgba(0,0,0,0.3); float: right; margin-top: 0.1rem; }}
  input[type="text"], input[type="number"] {{ width: 100%; padding: 0.4rem 0.2rem; background: transparent; border: none; border-bottom: 2px dashed rgba(0,0,0,0.15); font-family: 'Homemade Apple', cursive; font-size: 0.85rem; color: #2a2a2a; outline: none; transition: transform 0.2s, border-color 0.3s; }}
  input:focus {{ border-bottom-color: #ff006e; animation: inputWobble 0.3s ease; }}
  @keyframes inputWobble {{ 0% {{ transform: rotate(0); }} 25% {{ transform: rotate(-1deg); }} 50% {{ transform: rotate(1deg); }} 75% {{ transform: rotate(-0.5deg); }} 100% {{ transform: rotate(0); }} }}
  input::placeholder {{ color: rgba(0,0,0,0.25); font-family: 'Patrick Hand', cursive; font-style: italic; }}
  .photo-area {{ text-align: center; margin: 0.5rem 0; }}
  .photo-tabs {{ display: flex; gap: 0; justify-content: center; margin-bottom: 0.6rem; }}
  .photo-tab {{ padding: 0.35rem 1rem; font-family: 'Patrick Hand', cursive; font-size: 0.9rem; cursor: pointer; background: rgba(0,0,0,0.05); border: 2px solid rgba(0,0,0,0.1); color: #5a4a3a; transition: all 0.2s; }}
  .photo-tab:first-child {{ border-radius: 6px 0 0 6px; }}
  .photo-tab:last-child {{ border-radius: 0 6px 6px 0; }}
  .photo-tab.active {{ background: #1a1a2e; color: #ffe66d; border-color: #1a1a2e; }}
  .cam-box {{ position: relative; display: inline-block; background: #000; border-radius: 4px; overflow: hidden; box-shadow: 3px 3px 0 rgba(0,0,0,0.1); }}
  .cam-box video, .cam-box canvas {{ display: block; width: 220px; height: 220px; object-fit: cover; }}
  .cam-box canvas {{ display: none; }}
  .snap-btn {{ display: inline-block; margin-top: 0.5rem; padding: 0.4rem 1.2rem; background: #ff006e; color: #fff; font-family: 'Patrick Hand', cursive; font-size: 0.95rem; border: none; border-radius: 20px; cursor: pointer; box-shadow: 2px 2px 0 rgba(0,0,0,0.1); transition: transform 0.2s; }}
  .snap-btn:hover {{ transform: scale(1.06); }}
  .retake-btn {{ display: none; margin-top: 0.5rem; padding: 0.3rem 1rem; background: transparent; color: #7a6a5a; font-family: 'Patrick Hand', cursive; font-size: 0.85rem; border: 1px dashed #b0a090; border-radius: 15px; cursor: pointer; }}
  .upload-box {{ display: none; }}
  .file-label {{ display: inline-block; padding: 0.8rem 1.5rem; background: #fefcf3; border: 2px dashed #d4c4a8; border-radius: 8px; cursor: pointer; font-family: 'Patrick Hand', cursive; font-size: 0.95rem; color: #5a4a3a; transition: all 0.2s; }}
  .file-label:hover {{ border-color: #ff006e; background: #fff9f0; }}
  .file-preview {{ margin-top: 0.5rem; display: none; }}
  .file-preview img {{ width: 150px; height: 150px; object-fit: cover; border-radius: 4px; box-shadow: 2px 2px 0 rgba(0,0,0,0.08); }}
  .photo-skip {{ font-family: 'Homemade Apple', cursive; font-size: 0.6rem; color: #b0a090; margin-top: 0.4rem; }}
  .badge-row {{ display: flex; flex-wrap: wrap; gap: 0.5rem; margin-top: 0.8rem; margin-bottom: 0.4rem; justify-content: center; }}
  .lang-badge {{ cursor: pointer; transition: transform 0.2s, box-shadow 0.2s; }}
  .lang-badge input {{ display: none; }}
  .badge-face {{ display: inline-block; padding: 0.3rem 0.7rem; background: #fefcf3; border: 2px solid #d4c4a8; border-radius: 3px; font-family: 'Patrick Hand', cursive; font-size: 0.85rem; color: #4a3a2a; box-shadow: 2px 2px 0 rgba(0,0,0,0.06); transition: all 0.2s; user-select: none; }}
  .lang-badge:hover .badge-face {{ transform: scale(1.08); box-shadow: 3px 3px 0 rgba(0,0,0,0.1); }}
  .lang-badge input:checked + .badge-face {{ background: #1a1a2e; color: #ffe66d; border-color: #1a1a2e; box-shadow: 3px 3px 0 rgba(255,0,110,0.3); }}
  .lang-badge:focus .badge-face {{ outline: 2px dashed #ff006e; outline-offset: 3px; }}
  .submit-area {{ text-align: center; margin-top: 1.2rem; }}
  .submit-btn {{ display: inline-block; padding: 0.7rem 2.2rem; background: #1a1a2e; color: #ffe66d; font-family: 'Permanent Marker', cursive; font-size: 1.15rem; border: none; border-radius: 30px 6px 30px 6px; cursor: pointer; transform: rotate(-1.5deg); box-shadow: 4px 4px 0 rgba(0,0,0,0.15); transition: transform 0.2s, box-shadow 0.2s; }}
  .submit-btn:hover {{ transform: rotate(1deg) scale(1.06); box-shadow: 6px 6px 0 rgba(0,0,0,0.2); }}
  .submit-btn:active {{ transform: scale(0.96); }}
  .footer {{ text-align: center; margin-top: 0.8rem; font-family: 'Homemade Apple', cursive; font-size: 0.6rem; color: #b0a090; transform: rotate(0.5deg); }}
  .doodle-arrow {{ position: absolute; font-size: 1.8rem; opacity: 0.2; pointer-events: none; }}
  .scribble-divider {{ height: 12px; margin: 0.3rem 0; background: repeating-linear-gradient(90deg, transparent, transparent 4px, rgba(0,0,0,0.08) 4px, rgba(0,0,0,0.08) 8px); opacity: 0.5; border-radius: 2px; }}
  .field-error {{ font-family: 'Caveat', cursive; font-size: 0.95rem; color: #d94040; margin-top: 0.25rem; display: none; animation: errorShake 0.3s ease; }}
  .field-error.visible {{ display: block; }}
  .sticky.has-error {{ border: 2px solid #d94040; box-shadow: 0 0 0 2px rgba(217,64,64,0.15); }}
  @keyframes errorShake {{ 0%,100% {{ transform: translateX(0); }} 20% {{ transform: translateX(-4px); }} 40% {{ transform: translateX(4px); }} 60% {{ transform: translateX(-3px); }} 80% {{ transform: translateX(2px); }} }}
</style></head>
<body>
{splatter_html}
{sticker_html}
<div class="notebook">
  <div class="tape" style="top:-14px;left:25%;transform:rotate({tape_rot1}deg);"></div>
  <div class="tape" style="top:-14px;right:10%;transform:rotate({tape_rot2}deg);"></div>
  <div class="ink-smudge" style="bottom:30px;right:15px;"></div>
  <div class="ink-smudge" style="top:80px;left:35px;width:45px;height:30px;"></div>
  <div class="doodle-arrow" style="top:50px;right:-25px;transform:rotate(25deg);">&#10551;</div>
  <div style="text-align:center;">
    <div class="panda-mascot">&#128060;</div>
    <h1>{hl}</h1>
    <div class="subtitle">fill this out and something magical will happen... &#10024;</div>
  </div>
  <form method="POST" action="/website_name/CGI/contact.py" enctype="multipart/form-data" id="pandaForm">
    <div class="sticky" style="transform:rotate({sn0}deg);" id="s-name"><span class="field-label">&#9997; {name_prompt} <span class="char-hint">max 50</span></span><input type="text" name="name" placeholder="Bamboo McPandaface" aria-label="Your name"><div class="field-error" id="e-name"></div></div>
    <div class="sticky blue" style="transform:rotate({sn1}deg);" id="s-age"><span class="field-label">&#127874; {age_prompt}</span><input type="number" name="age" placeholder="25" aria-label="Your age"><div class="field-error" id="e-age"></div></div>
    <div class="sticky pink" style="transform:rotate({sn2}deg);" id="s-occ"><span class="field-label">&#128188; {job_prompt} <span class="char-hint">max 80</span></span><input type="text" name="occupation" placeholder="Professional napper" aria-label="Your occupation"><div class="field-error" id="e-occ"></div></div>
    <div class="sticky green" style="transform:rotate({sn3}deg);" id="s-hob"><span class="field-label">&#127918; {hobby_prompt} <span class="char-hint">max 80</span></span><input type="text" name="hobby" placeholder="Competitive bamboo juggling" aria-label="Your hobby"><div class="field-error" id="e-hob"></div></div>
    <div class="scribble-divider"></div>
    <div class="sticky purple" style="transform:rotate({sn4}deg);">
      <span class="field-label">&#128248; show us your face (optional)</span>
      <div class="photo-area">
        <div class="photo-tabs">
          <div class="photo-tab active" id="tabCam" onclick="switchTab('cam')">&#128247; selfie</div>
          <div class="photo-tab" id="tabFile" onclick="switchTab('file')">&#128193; upload</div>
        </div>
        <div id="camMode">
          <div id="camOff" style="text-align:center;"><button type="button" class="snap-btn" onclick="enableCamera()" style="background:#5a4a3a;">&#128247; turn on camera</button></div>
          <div id="camOn" style="display:none;">
            <div class="cam-box"><video id="camVideo" autoplay playsinline muted></video><canvas id="camCanvas"></canvas></div><br>
            <button type="button" class="snap-btn" id="snapBtn" onclick="snapPhoto()">&#128248; snap!</button>
            <button type="button" class="retake-btn" id="retakeBtn" onclick="retakePhoto()">retake</button>
          </div>
        </div>
        <div class="upload-box" id="fileMode">
          <label class="file-label" for="fileInput">&#128206; drag &amp; drop or click to choose a photo</label>
          <input type="file" id="fileInput" accept="image/jpeg,image/png,image/gif" style="display:none;" onchange="previewFile(this)">
          <div class="file-preview" id="filePreview"><img id="filePreviewImg" src="" alt="preview"></div>
        </div>
        <div class="photo-skip">no photo? the panda will imagine your face instead &#128060;&#128173;</div>
      </div>
    </div>
    <input type="file" name="photo" id="photoInput" style="display:none;" accept="image/*">
    <div class="scribble-divider"></div>
    <div style="text-align:center;">
      <span class="field-label" style="margin-top:0.5rem;">&#128187; pick your weapon:</span>
      <div class="badge-row" role="radiogroup" aria-label="Programming language">{badge_html}</div>
      <div id="customLangBox" style="display:none;margin-top:0.5rem;"><input type="text" name="custom_language" id="customLangInput" placeholder="type your language..." maxlength="30" style="width:60%;text-align:center;" aria-label="Custom programming language"></div>
    </div>
    <div class="submit-area"><button type="submit" class="submit-btn" id="submitBtn">&#128062; {submit_text}</button></div>
  </form>
  <div class="footer">we promise not to share your data with other pandas &#128060;&#129305;</div>
</div>
<script>
var stream = null, snapped = false, camStarted = false;

function switchTab(mode) {{
  document.getElementById('tabCam').classList.toggle('active', mode === 'cam');
  document.getElementById('tabFile').classList.toggle('active', mode === 'file');
  document.getElementById('camMode').style.display = mode === 'cam' ? 'block' : 'none';
  document.getElementById('fileMode').style.display = mode === 'file' ? 'block' : 'none';
  if (mode === 'cam') {{
    document.getElementById('fileInput').value = '';
    document.getElementById('filePreview').style.display = 'none';
    clearPhotoInput();
  }} else {{
    stopCamera();
    if (!snapped) {{ document.getElementById('camOff').style.display = 'block'; document.getElementById('camOn').style.display = 'none'; camStarted = false; }}
  }}
}}

function enableCamera() {{
  document.getElementById('camOff').style.display = 'none';
  document.getElementById('camOn').style.display = 'block';
  camStarted = true;
  startCamera();
}}

function startCamera() {{
  if (stream) return;
  var video = document.getElementById('camVideo');
  navigator.mediaDevices.getUserMedia({{ video: {{ facingMode: 'user', width: 440, height: 440 }} }})
    .then(function(s) {{ stream = s; video.srcObject = s; }})
    .catch(function() {{
      document.getElementById('camOn').innerHTML = '<div style="padding:1rem;color:#8a7a6a;font-family:Caveat,cursive;font-size:1.1rem;">&#128247; camera not available<br><small>switch to upload tab instead</small></div>';
    }});
}}

function stopCamera() {{
  if (stream) {{ stream.getTracks().forEach(function(t) {{ t.stop(); }}); stream = null; }}
}}

function snapPhoto() {{
  var video = document.getElementById('camVideo'), canvas = document.getElementById('camCanvas');
  canvas.width = 440; canvas.height = 440;
  var ctx = canvas.getContext('2d');
  ctx.translate(440, 0); ctx.scale(-1, 1);
  ctx.drawImage(video, 0, 0, 440, 440);
  ctx.setTransform(1, 0, 0, 1, 0, 0);
  video.style.display = 'none'; canvas.style.display = 'block';
  document.getElementById('snapBtn').style.display = 'none';
  document.getElementById('retakeBtn').style.display = 'inline-block';
  snapped = true; stopCamera();
  var dataUrl = canvas.toDataURL('image/jpeg', 0.85);
  var byteStr = atob(dataUrl.split(',')[1]);
  var ab = new ArrayBuffer(byteStr.length), ia = new Uint8Array(ab);
  for (var j = 0; j < byteStr.length; j++) ia[j] = byteStr.charCodeAt(j);
  var file = new File([new Blob([ab], {{ type: 'image/jpeg' }})], 'selfie.jpg', {{ type: 'image/jpeg' }});
  var dt = new DataTransfer(); dt.items.add(file);
  document.getElementById('photoInput').files = dt.files;
}}

function retakePhoto() {{
  var video = document.getElementById('camVideo'), canvas = document.getElementById('camCanvas');
  video.style.display = 'block'; canvas.style.display = 'none';
  document.getElementById('snapBtn').style.display = 'inline-block';
  document.getElementById('retakeBtn').style.display = 'none';
  snapped = false; clearPhotoInput(); startCamera();
}}

function clearPhotoInput() {{ var dt = new DataTransfer(); document.getElementById('photoInput').files = dt.files; }}

function previewFile(input) {{
  if (input.files && input.files[0]) {{
    var reader = new FileReader();
    reader.onload = function(e) {{ document.getElementById('filePreviewImg').src = e.target.result; document.getElementById('filePreview').style.display = 'block'; }};
    reader.readAsDataURL(input.files[0]);
    var dt = new DataTransfer(); dt.items.add(input.files[0]);
    document.getElementById('photoInput').files = dt.files;
  }}
}}

function showCustomLang(show) {{
  document.getElementById('customLangBox').style.display = show ? 'block' : 'none';
  if (show) document.getElementById('customLangInput').focus();
  else document.getElementById('customLangInput').value = '';
}}

function setErr(id, stickyId, msg) {{
  var el = document.getElementById(id), st = document.getElementById(stickyId);
  if (msg) {{
    el.textContent = msg; el.classList.add('visible'); st.classList.add('has-error');
  }} else {{
    el.textContent = ''; el.classList.remove('visible'); st.classList.remove('has-error');
  }}
  return !msg;
}}

function validateForm() {{
  var ok = true;
  var name = document.querySelector('input[name="name"]').value.trim();
  var age = document.querySelector('input[name="age"]').value.trim();
  var occ = document.querySelector('input[name="occupation"]').value.trim();
  var hob = document.querySelector('input[name="hobby"]').value.trim();

  if (!name) {{ ok = setErr('e-name','s-name','the panda needs your name!') && ok; }}
  else if (name.length > 50) {{ ok = setErr('e-name','s-name','too long! max 50 characters (you have ' + name.length + ')') && ok; }}
  else {{ setErr('e-name','s-name',''); }}

  if (!age) {{ ok = setErr('e-age','s-age','how old are you? the panda must know') && ok; }}
  else if (isNaN(parseInt(age)) || parseInt(age) < 1 || parseInt(age) > 120) {{ ok = setErr('e-age','s-age','age must be between 1 and 120. no immortals allowed.') && ok; }}
  else {{ setErr('e-age','s-age',''); }}

  if (!occ) {{ ok = setErr('e-occ','s-occ','what do you do? even "nothing" counts') && ok; }}
  else if (occ.length > 80) {{ ok = setErr('e-occ','s-occ','too long! max 80 characters (you have ' + occ.length + ')') && ok; }}
  else {{ setErr('e-occ','s-occ',''); }}

  if (!hob) {{ ok = setErr('e-hob','s-hob','no hobby? that makes the panda sad') && ok; }}
  else if (hob.length > 80) {{ ok = setErr('e-hob','s-hob','too long! max 80 characters (you have ' + hob.length + ')') && ok; }}
  else {{ setErr('e-hob','s-hob',''); }}

  return ok;
}}

document.getElementById('pandaForm').addEventListener('submit', function(e) {{
  if (!validateForm()) {{ e.preventDefault(); return; }}
  var btn = document.getElementById('submitBtn');
  btn.disabled = true; btn.textContent = '\\uD83D\\uDC3E panda is thinking...'; btn.style.opacity = '0.7';
}});

// Clear errors on input
document.querySelectorAll('#pandaForm input[type="text"], #pandaForm input[type="number"]').forEach(function(inp) {{
  inp.addEventListener('input', function() {{
    var st = inp.closest('.sticky');
    if (st && st.classList.contains('has-error')) {{
      var errEl = st.querySelector('.field-error');
      if (errEl) {{ errEl.textContent = ''; errEl.classList.remove('visible'); }}
      st.classList.remove('has-error');
    }}
  }});
}});

document.querySelectorAll('.lang-badge').forEach(function(b) {{
  b.addEventListener('keydown', function(e) {{ if (e.key === 'Enter' || e.key === ' ') {{ e.preventDefault(); b.querySelector('input').checked = true; }} }});
}});
</script>
{parallax_js}
</body></html>""".format(**ctx))
