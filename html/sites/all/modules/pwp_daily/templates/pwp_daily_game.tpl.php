<script id="app-template" type="x-template">
  <div>
    <template v-if="loading">
      Loading...
    </template>
    <template v-else-if="error">

    </template>
    <template v-else>
      <component
        :is="component"
        :show="show"
        :matches="show.matches"
        @game-start="handleGameStart"
      />
    </template>
  </div>
</script>

<script id="game-intro-template" type="x-template">
  <div>
    <h2>{{ show.title }}</h2>
    <p>{{ show.type.name }}</p>
    <p>{{ show.venue.name }}</p>

    <div class="button">
      <a href="#" @click.prevent="$emit('game-start')">Start</a>
    </div>
  </div>
</script>

<script id="game-template" type="x-template">
  <div>
    <match
      v-for="match in matches"
      :key="match.id"
      :id="match.id"
      :title="match.title"
      :teams="match.teams"
    />
  </div>
</script>

<script id="match-template" type="x-template">
  <div>
    <h2>{{ title }}</h2>

    <team
      v-for="(team, key, index) in teams"
      v-model="selected"
      :key="getKey(key)"
      :id="id"
      :checked="getKey(key) === selected"
      :team="key"
      :wrestlers="team"
    >
      <template
        v-if="shouldAppend(index)"
        #append
      >
        VS
      </template>
    </team>
  </div>
</script>

<script id="team-template" type="x-template">
  <div>
    <slot name="prepend">
      <input
        type="checkbox"
        :checked="checked"
        @input="handleInput"
      />
    </slot>

    <ul>
      <li
        v-for="wrestler in wrestlers"
      >
        {{ wrestler.title }}
      </li>
    </ul>

    <slot name="append" />
  </div>
</script>

<div id="daily_game"></div>
